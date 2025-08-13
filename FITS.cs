using System.Text;

namespace FITSProcessor
{
    internal class FITS
    {
        static int bitpix = 0;
        static int[] axis = null;
        static string extname = string.Empty;

        static string ReadHeader(BinaryReader br)
        {
            bitpix = 0;
            axis = null;
            extname = string.Empty;
            byte[] block = new byte[2880];
            StringBuilder sb = new StringBuilder();
            br.Read(block, 0, block.Length);
            string hr = System.Text.Encoding.ASCII.GetString(block[0..80]).Trim();
            sb.AppendLine(hr);

            for (int i = 80; i <= block.Length; i += 80)
            {
                if (i == block.Length)
                {
                    br.Read(block, 0, block.Length);
                    i = 0;
                }

                hr = System.Text.Encoding.ASCII.GetString(block[i..(i + 80)]).Trim();
                sb.AppendLine(hr);

                if (hr.StartsWith("END"))
                    break;

                if (hr.StartsWith("EXTNAME = '"))
                    extname = hr[11..hr.IndexOf("'", 12)].Trim();
                else if (hr.StartsWith("BITPIX  = "))
                    bitpix = int.Parse(hr[10..30].Trim());
                else if (hr.StartsWith("NAXIS"))
                {
                    if (hr[5] == ' ')
                    {
                        int naxis = int.Parse(hr[10..30].Trim());
                        if (naxis > 0)
                            axis = new int[naxis];
                    }
                    else if (Char.IsDigit(hr[5]))
                    {
                        int ai = (int)hr[5] - (int)'1';
                        if (ai >= 0 && ai < axis.Length)
                            axis[ai] = int.Parse(hr[10..30].Trim());
                    }
                }
            }

            return sb.ToString();
        }

        static void HexDump(BinaryReader br, StreamWriter sw)
        {
            long dataBytes = (long)(Math.Abs(bitpix) / 8);
            foreach (int ax in axis)
                dataBytes *= (long)ax;

            StringBuilder sb = new StringBuilder();
            for (long i = 0; i < dataBytes; i++)
            {
                if ((i & 15L) == 0L)
                    sw.Write($"{i.ToString("X8")} ");

                byte val = br.ReadByte();
                sw.Write($"{val.ToString("X2")} ");
                sb.Append((val >= 32 && val < 127) ? Convert.ToChar(val) : '.');

                if ((i & 15L) == 15L)
                {
                    sw.WriteLine(sb.ToString());
                    sb.Clear();
                }
                else if (i == dataBytes)
                {
                    while ((i++ & 15L) != 15L)
                        sw.Write("   ");

                    sw.WriteLine(sb.ToString());
                }
            }

            //block align the binary reader
            long hanging = (br.BaseStream.Position % 2880L);
            if (hanging > 0L)
                br.BaseStream.Seek(2880L - hanging, SeekOrigin.Current);
        }

        internal static void DataDump(string fullFilePath)
        {
            string basepath = Path.Combine(Path.GetDirectoryName(fullFilePath), $"{Path.GetFileNameWithoutExtension(fullFilePath)}");
            using BinaryReader br = new(File.OpenRead(fullFilePath));
            while (br.BaseStream.Position < (br.BaseStream.Length - 2880L))
            {
                string header = ReadHeader(br);
                string outpath = basepath + ((header.StartsWith("SIMPLE")) ? "_simple.txt" : $"_{extname}.txt");
                using (StreamWriter sw = new(outpath))
                {
                    sw.Write(header);
                    if (axis != null)
                        HexDump(br, sw);
                }
            }
        }

        internal static List<string> GetFileHeaders(string fullFilePath)
        {
            List<string> result = new();
            using BinaryReader br = new(File.OpenRead(fullFilePath));
            while (br.BaseStream.Position < (br.BaseStream.Length - 2880L))
            {
                string header = ReadHeader(br);
                result.Add(header);
                if (header.StartsWith("SIMPLE"))
                    continue;
                else if (!string.IsNullOrEmpty(extname) && axis != null)
                {
                    long dataBytes = (long)(Math.Abs(bitpix) / 8);
                    foreach (int ax in axis)
                        dataBytes *= (long)ax;

                    //block align the databytes count
                    long hanging = (dataBytes % 2880L);
                    if (hanging > 0L)
                        dataBytes += (2880L - hanging);

                    br.BaseStream.Seek(dataBytes, SeekOrigin.Current);
                }
                else
                    break;
            }

            return result;
        }

        internal static FITSImage GetImage(string fullFilePath, string hduName, int histogramWidth)
        {
            FITSImage image = null;
            string fileheader = string.Empty;
            using BinaryReader br = new(File.OpenRead(fullFilePath));
            while (br.BaseStream.Position < (br.BaseStream.Length - 2880L))
            {
                string header = ReadHeader(br);
                if (header.StartsWith("SIMPLE"))
                    fileheader = header;
                else if (extname == hduName && axis.Length == 2)
                {
                    image = new(fullFilePath, fileheader, header, axis[0], axis[1], histogramWidth);
                    image.ReadImageData(br);
                    image.Reset();
                    break;
                }
                else if (axis != null)
                {
                    long dataBytes = (long)(Math.Abs(bitpix) / 8);
                    foreach (int ax in axis)
                        dataBytes *= (long)ax;

                    //block align the databytes count
                    long hanging = (dataBytes % 2880L);
                    if (hanging > 0L)
                        dataBytes += (2880L - hanging);

                    br.BaseStream.Seek(dataBytes, SeekOrigin.Current);
                }
            }

            return image;
        }

        internal static bool GetHeaderString(string header, string name, out string result)
        {
            result = string.Empty;
            int i = header.IndexOf(name);
            if (i >= 0)
            {
                i = header.IndexOf("'", i) + 1;
                result = header[i..(header.IndexOf("'", i))].Trim();
                return true;
            }

            return false;
        }

        internal static bool GetHeaderDouble(string header, string name, out double result)
        {
            result = double.NaN;
            int i = header.IndexOf(name);
            if (i >= 0)
            {
                string text = header[(i + 9)..(i + 31)].Trim();
                return double.TryParse(text, out result);
            }

            return false;
        }


        internal static bool GetHeaderInteger(string header, string name, out int result)
        {
            result = -1;
            int i = header.IndexOf(name);
            if (i >= 0)
            {
                string text = header[(i + 9)..(i + 31)].Trim();
                return int.TryParse(text, out result);
            }

            return false;
        }


        internal static bool GetHeaderBoolean(string header, string name, out bool result)
        {
            result = false;
            int i = header.IndexOf(name);
            if (i >= 0)
            {
                string text = header[(i + 9)..(i + 31)].Trim();
                result = (text == "T");
                return true;
            }

            return result;
        }
    }
}
