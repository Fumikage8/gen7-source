using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Drawing;
using System.Drawing.Imaging;

namespace PokeViewerHost.Utility
{
    public class PokeDexImageTool
    {
        /*
         * 
         * */
        public static Bitmap CreateImageFromFile(string path, bool isRotate270)
        {
            Bitmap bmp = null;


            bmp = new Bitmap(path);

            if (isRotate270)
            {
                bmp.RotateFlip(RotateFlipType.Rotate270FlipNone);
            }

            return bmp;
        }


        /*
         * 入力BMPをアルファイメージとして、輝度を反転して出力
         * */
        public static Bitmap CreateInvAlphaImage(Bitmap sourceAlpha)
        {

            Bitmap alphaBmp = null;


            try
            {
                alphaBmp = new Bitmap(sourceAlpha);

                /*   lock   */
                BitmapData alphaData = alphaBmp.LockBits(new Rectangle(0, 0, alphaBmp.Width, alphaBmp.Height), ImageLockMode.ReadOnly, alphaBmp.PixelFormat);

                {
                    byte[] alphaBytes = new byte[alphaData.Stride * alphaData.Height];

                    System.Runtime.InteropServices.Marshal.Copy(alphaData.Scan0, alphaBytes, 0, alphaBytes.Length);

                    for (int y = 0; y < alphaBmp.Height; ++y)
                    {
                        int index_alpha = y * alphaData.Stride;

                        for (int x = 0; x < alphaBmp.Width; ++x)
                        {
                            int offset_alpha = x * 4;
                            int index = index_alpha + offset_alpha;
                            /*
                                alphaBytes[index + 0] = (byte)(0xff - alphaBytes[index + 0]);
                                alphaBytes[index + 1] = (byte)(0xff - alphaBytes[index + 1]);
                                alphaBytes[index + 2] = (byte)(0xff - alphaBytes[index + 2]);
                            */
                            {
                                double b = 0xff - alphaBytes[index + 0];
                                double g = 0xff - alphaBytes[index + 1];
                                double r = 0xff - alphaBytes[index + 2];
                                int luminance = (int)((0.298912 * r) + (0.586611 * g) + (0.114478 * b));

                                alphaBytes[index + 0] =
                                alphaBytes[index + 1] =
                                alphaBytes[index + 2] = (byte)Math.Min(0xff, luminance);
                            }
                        }
                    }


                    System.Runtime.InteropServices.Marshal.Copy(alphaBytes, 0, alphaData.Scan0, alphaBytes.Length);
                }
                /*   unlock   */
                alphaBmp.UnlockBits(alphaData);

            }
            catch (Exception e)
            {
//                Console.WriteLine(e.ToString());
            }
            
            return alphaBmp;
        }

        /*
         * カラーイメージとアルファイメージを合成
         * */
        public static Bitmap CreateCompositeImage(Bitmap sourceColorBmp, Bitmap sourceAlphaBmp)
        {
            if((sourceColorBmp == null) || (sourceAlphaBmp == null))  return null;


            Bitmap compositeBmp = sourceColorBmp.Clone(new Rectangle(new Point(), sourceColorBmp.Size), PixelFormat.Format32bppArgb);

            {
                BitmapData colorData = compositeBmp.LockBits(new Rectangle(0, 0, compositeBmp.Width, compositeBmp.Height), ImageLockMode.WriteOnly, compositeBmp.PixelFormat);
                BitmapData alphaData = sourceAlphaBmp.LockBits(new Rectangle(0, 0, sourceAlphaBmp.Width, sourceAlphaBmp.Height), ImageLockMode.ReadOnly, sourceAlphaBmp.PixelFormat);

                byte[] colorBytes = new byte[colorData.Stride * colorData.Height];
                byte[] alphaBytes = new byte[alphaData.Stride * alphaData.Height];

                System.Runtime.InteropServices.Marshal.Copy(colorData.Scan0, colorBytes, 0, colorBytes.Length);
                System.Runtime.InteropServices.Marshal.Copy(alphaData.Scan0, alphaBytes, 0, alphaBytes.Length);

                for (int y = 0; y < colorData.Height; ++y)
                {
                    int index_color = y * colorData.Stride;
                    int index_alpha = y * alphaData.Stride;

                    for (int x = 0; x < colorData.Width; ++x)
                    {
                        int offset_color = x * 4;
                        int offset_alpha = x * 4;

                        colorBytes[index_color + offset_color + 3] = alphaBytes[index_alpha + offset_alpha + 0];
                    }
                }


                System.Runtime.InteropServices.Marshal.Copy(colorBytes, 0, colorData.Scan0, colorBytes.Length);

                compositeBmp.UnlockBits(colorData);
                sourceAlphaBmp.UnlockBits(alphaData);
            }

            return compositeBmp;
        }
    }
}
