using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Drawing;
using System.Drawing.Imaging;

namespace ConcatImage
{
    class Program
    {
        int Result { get; set; }



        static void Main(string[] args)
        {
            int result = 0;

            {
                System.Reflection.Assembly  asm = System.Reflection.Assembly.GetExecutingAssembly();

                Log(@"{0}: {1}", asm.GetName().Name, asm.GetName().Version);
                foreach(string arg in args)
                {
                    Log(@"  {0}", arg);
                }
            }


            try
            {
                string[] inFilePaths = new string[args.Length - 1];
                string outFilePath = args[args.Length - 1];
                


                for (int index = 0; index < inFilePaths.Length; ++index)
                {
                    inFilePaths[index] = args[index];
                }



                Bitmap outBmp = null;

                foreach (string inFilePath in inFilePaths)
                {
                    Bitmap oldBmp = outBmp;
                    using(Bitmap inBmp = new Bitmap(inFilePath))
                    {
                        int w = (oldBmp != null) ? oldBmp.Size.Width : inBmp.Size.Width;
                        int h = (oldBmp != null) ? (oldBmp.Size.Height + inBmp.Size.Height) : inBmp.Size.Height;
                        int y = (oldBmp != null) ? oldBmp.Height : 0;

                        outBmp = new Bitmap(w, h, inBmp.PixelFormat);
                        outBmp.SetResolution(inBmp.HorizontalResolution, inBmp.VerticalResolution);
                        if (oldBmp != null)
                        {
                            using (Graphics g = Graphics.FromImage(outBmp))
                            {
                                g.CompositingMode = System.Drawing.Drawing2D.CompositingMode.SourceCopy;
                                g.DrawImage(oldBmp, 0, 0);
                            }
                            oldBmp.Dispose();
                        }

                        /*      */
                        using(Graphics g = Graphics.FromImage(outBmp))
                        {
                            g.CompositingMode = System.Drawing.Drawing2D.CompositingMode.SourceCopy;

                            g.DrawImage(inBmp, new Point(0, y));
                        }
                    }
                }

                outBmp.Save(outFilePath,ImageFormat.Bmp);
                outBmp.Dispose();
            }
            catch (Exception e)
            {
                Log(e.ToString());
                result = 1;
            }
            finally
            {
                Environment.Exit(result);
            }

        }

        static void Log(string format, params object[] parameters)
        {
            Console.WriteLine(format, parameters);
        }

    }
}
