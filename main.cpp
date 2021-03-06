//#include "mainwindow.h"
//#include <QApplication>
#include <iostream>
#include<stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include"feature_option.h"
//#include"io.h"
#include"feat_desc.h"
extern "C" {
  #include <vl/generic.h>
#include<vl/dsift.h>
}

using namespace std;
using namespace cv;

int main(int argc, char *argv[])
{
    VL_PRINT ("Hello world!") ;
    if( argc != 2)
        {
         cout <<" Usage: display_image ImageToLoadAndDisplay" << endl;
         return -1;
        }
      VL_PRINT ("Hello world!") ;
     cout << "start";


        Mat image,grayIm;
        image = imread("w3_4.jpg");   // Read the file
        cout << "image loaded";

        if(! image.data )         // Check for invalid input
        {
            cout <<  "Could not open or find the image" << std::endl ;
            return -1;
        }
        cvtColor(image, grayIm, cv::COLOR_BGR2GRAY);


        int k=0;
        float *grayImg=(float*)malloc(grayIm.rows*grayIm.cols*sizeof(float));
        for (int i = 0; i < grayIm.rows; ++i)
        {
          for (int j = 0; j < grayIm.cols; ++j)
          {
            grayImg[k]= grayIm.at<unsigned char>(i, j);
            grayImg[k]= grayImg[k]/255;
            k++;
          }
        }

//        FILE *ftest;
//                ftest = fopen("test2.txt","w");
//                //uchar *data =grayIm.data;
//       k=0;
//        for (int i = 0; i < grayIm.rows; ++i)
//        {
//            fprintf(ftest,"\n");
//          for (int j = 0; j < grayIm.cols; ++j)
//          {
//              int bin =grayImg[k++]>127;
//              fprintf(ftest,"%d", bin);
//          }
//        }
//fclose(ftest);

        featParams param =featParams();
        pathParam resPath = pathParam();


        //int featDim;
        //prepare_opts(param,resPath);
        //printf("aqui");

//        /* calling dsift function of vl_feat library*/
        denseSift featVec;
        featVec.descrs =Mat::zeros(1,128,CV_32FC1);
        featVec.frames = Mat::zeros(1,4,CV_64FC1);

        get_vl_phow(featVec.descrs,featVec.frames,param,grayImg,grayIm.rows,grayIm.cols);
        featVec.descrSize = featVec.descrs.cols;
        featVec.numFrames = featVec.descrs.rows;
        printf(" In main %d %d\n",featVec.descrs.rows,featVec.descrs.cols);

        FILE *fdes = fopen("descriptor.txt","w");
        //FILE *ftest2 = fopen("testmatframe.txt","w");
                         for (int i = 0; i <featVec.descrs.rows; ++i)
                         {
                             fprintf(fdes,"\n");
                           for (int j = 0; j < featVec.descrs.cols; ++j)
                             fprintf(fdes,"%f ",featVec.descrs.at<float>(i, j));
                         }
                         fclose(fdes);

//normalize sift is due

//        resPath.lexPath ="lexicon.bin";
//        resPath.pcaPath ="PCA.bin";
//        resPath.gmmPath ="GMM.bin";
//        resPath.attsPath ="attModels.bin";
//        resPath.ccaPath="CCA.bin";
        /* reading PCA file from matlab*/
        pcaTemp PCAModel = pcaTemp(resPath.pcaPath);
        FILE *ftest1;
        ftest1 = fopen("testPCAMain.txt","w");
       // GM1M->we = ConvertToMat(GMM->refWe,1,GMM->G);
        fprintf(ftest1,"means\n");
      for (int iter=0;iter<PCAModel.dim;iter++)
        fprintf(ftest1,"%f \n",PCAModel.mean.at<float>(0,iter));
      fprintf(ftest1,"eigvec\n");
      for (int iter=0;iter<PCAModel.num;iter++)
      {
          fprintf(ftest1,"\n");
          for (int iter1=0;iter1<PCAModel.dim;iter1++)
              fprintf(ftest1,"%f ",PCAModel.eigvec.at<float>(iter,iter1));
      }
        fclose(ftest1);

        printf("PCA attributes \n%d %d\n",PCAModel.dim,PCAModel.num);

        /* reading GMM file from Matlab*/
        //GMMTemp *GMM =new GMMTemp();
        GMMTemp GMM =GMMTemp(resPath.gmmPath);
        printf("\nGMM attributes %d %d\n",GMM.D,GMM.G);
        // printf("%u\n",GMM.we.data);
        FILE *ftest;
        ftest = fopen("testGMMMain.txt","w");
        //GMM->we = ConvertToMat(GMM->refWe,1,GMM->G);
      for (int iter=0;iter<GMM.G;iter++)
        fprintf(ftest,"%f \n",GMM.we.at<float>(0,iter));
        //fclose(ftest);

        for(int iter =0;iter<GMM.G;iter++)
        {
            fprintf(ftest,"\n");
            for(int iter1=0;iter1<GMM.D;iter1++)
            {
                fprintf(ftest,"%f ",GMM.mu.at<float>(iter,iter1));
            }

        }

        fclose(ftest);


        /* calling vl_fisher function of vl_feat library to encode the SIFT vectors */
        Mat FV;
        get_vl_fisher_encode(FV,featVec,GMM,PCAModel);
       // Mat FV;
        printf("in main");

        //printf("%d %d\n",FV.rows,FV.cols);
       // Mat FV = Mat::zeros(1,param.featDim,DataType<float>::type);
//        FV.data = fv;

//         read embedding matrix and CCA matrix */
        int N,D;

       readMatDim(N,D,resPath.attsPath);
        float *emb = (float*)malloc(sizeof(float)*N*D);
        Mat W = Mat::zeros(N,D,DataType<float>::type);
        readMatData(emb,resPath.attsPath);
        W =ConvertToMat(emb,N,D,CV_32FC1);
        //embW.data =W;
       printf("\n%d %d\n",W.rows,W.cols);
       FILE *ftestemb = fopen("textEmb.txt","w");

       for(int iter =0;iter<W.rows;iter++)
       {
           fprintf(ftestemb,"\n");
           for(int iter1=0;iter1<W.cols;iter1++)
           {
               fprintf(ftestemb,"%f ",W.at<float>(iter,iter1));
           }

       }

       fclose(ftestemb);

        CCATemp CCA = CCATemp(resPath.ccaPath);
        //Mat CCA = Mat::zeros(param.featDim,param.numAtts,DataType<float>::type);

        //CCA.data = cca;

        W =W.colRange(0,FV.rows);
        printf("\nFV dim %d %d\n W dim %d %d\nCCA dim %d %d",FV.rows,FV.cols,W.rows,W.cols,CCA.Wx.rows,CCA.Wx.cols);
//        /* multiply fv,embedding matrix,cca matrix like atts = (fv*W')*CCA' */
        Mat atts =W*FV;// FV* CCA.Wx;
        printf("\n%d %d\n",atts.rows,atts.cols);
        atts =CCA.Wx.t() *atts;
        printf("\n%d %d\n",atts.rows,atts.cols);
//        /* take l2 norm */

//        /* read a lexicon file precomputed */
//        float *lex = readLexicon(resPath.lexPath);
        readMatDim(N,D,resPath.lexPath);
         float *lex = (float*)malloc(sizeof(float)*N*D);
         Mat lexmat = Mat::zeros(N,D,DataType<float>::type);
         readMatData(lex,resPath.attsPath);
         lexmat =ConvertToMat(lex,N,D,CV_32FC1);
         //embW.data =W;
        printf("\n%d %d\n",lexmat.rows,lexmat.cols);
        FILE *ftestPhoc = fopen("textEmb.txt","w");

        for(int iter =0;iter<lexmat.rows;iter++)
        {
            fprintf(ftestemb,"\n");
            for(int iter1=0;iter1<lexmat.cols;iter1++)
            {
                fprintf(ftestemb,"%f ",lexmat.at<float>(iter,iter1));
            }

        }
        fclose(ftestPhoc);
printf("here before score");
//        /* take a dot product in matlab lex*atts'*/

        Mat S =  (lexmat*CCA.Wy)*atts;
        printf("%d, %d",S.rows,S.cols);
        FILE *fscore = fopen("score.txt","w");//printf()
        for(int iter =0;iter<S.rows;iter++)

        for(int iter1=0;iter1<S.cols;iter1++)

            fprintf(fscore,"%f\n ",S.at<float>(iter,iter1));
fclose(fscore);

//release all mat and allocated data

        /* sort and get the result */










        // Create a window for display.
        //namedWindow( "Display window" );

//        // Show our image inside it.
      // imshow( "Display window", grayIm );

       // waitKey(0);              // Wait for a keystroke in the window
        return 0;
}


//int main (int argc, const char * argv[]) {
//  VL_PRINT ("Hello world!") ;
//  return 0;
//}
