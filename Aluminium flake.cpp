#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <stdio.h>
#include <sstream>

using namespace std;
using namespace cv;


Mat src = imread("HAHA2.jpg"),cloneimg,new_image;
bool mousedown;   
Mat masked(src.size(),CV_8UC3,Scalar(255,255,255));  
Mat masked1,masked3,dst,binary,src_HSV,frame_threshold;       

vector<Point> pts;


int thresh = 100;
int B_thresh=60;
int max_thresh = 255;
RNG rng(12345);
int areathresh=0;
int maxarea_thresh;
int c=0,b,g,r;
double aspect_ratio;  
void arearesult(int ,void*);   
Mat drawing;   

//------------------------------ BINARY TRACKBAR CALL BACK FUNCTION -------------------------------------------------------------

Mat makebinary(Mat imageBinary, int thresh)

{

    Mat result(imageBinary.size(),CV_8UC1);
    for(int i = 0 ; i < imageBinary.rows ; i++)
    {
        for(int j = 0 ; j < imageBinary.cols ; j++)
        {
            if(imageBinary.at<uchar>(i,j)>thresh)
            
                result.at<uchar>(i,j)=0;    //Make pixel black
            
            else
            
           	    result.at<uchar>(i,j)=255;  //Make pixel white
       
        }
        
    }
    return result;
}

//---------------------------------------------------MAIN FUNCTION --------------------------------------------------------------


int main( int argc, const char** argv )
{

   	if(src.empty())
    	{
        	return -1;
    	}

    	
    	
    	
      namedWindow("Binary",WINDOW_NORMAL);
    	
    	resizeWindow("Binary",600,600);
    


    	Mat cloneBinary= src.clone();


    	createTrackbar("Threshold","Binary",&thresh, max_thresh);                               //TRACKBAR TO SET THRESHOLD FOR BINARY

    	
		  Mat gray;
    	
      while(1)                                                                                //TO GENERATE BINARY IMAGE FROM TRACKBAR
   	  {
        cvtColor(cloneBinary,gray,CV_BGR2GRAY);
       
        binary=makebinary(gray,thresh);
        
        imshow("Binary",binary);
        
        char a = waitKey(33);
        
        if(a=='c' || a=='C')
        {
          
          destroyWindow("Binary");    
          break;
        
        }
        
      }
           

      Mat element = getStructuringElement( MORPH_ELLIPSE ,Size(5,5) );

      //erode( binary, binary, element );

      vector<vector<Point> > contours;
      findContours( binary, contours, RETR_CCOMP, CHAIN_APPROX_SIMPLE );                      // FIND CONTOURS ON IMAGE

      int n=contours.size();

      vector<vector<Point> > contours_poly1(n );
      vector<Rect> boundRect1( n );
      vector<Point2f>centers1( n );
      vector<float>radius1( n ); 
      double area_array[n];
      double ratio[n];
      double radius[n];
      ostringstream str1; 



      for( size_t i =0; i < contours.size(); i++ )
      {
                	approxPolyDP( contours[i], contours_poly1[i], 3, true );
                	boundRect1[i] = boundingRect( contours_poly1[i] );
                	minEnclosingCircle( contours_poly1[i], centers1[i], radius1[i] );
      }

      maxarea_thresh=0; 
      
         
 //---------------------------draw ALL contours on image and calculate max area of contour---------------------------------------



      Mat all_contour = Mat( src.size(), CV_8UC3,Scalar(224,224,224) );        

      for( size_t i = 0; i< contours.size(); i++ )                                           
      {

          drawContours( all_contour, contours, (int)i,Scalar(0,103,0));
        
          if (contourArea(contours[i])>maxarea_thresh )
          {
    	               maxarea_thresh=contourArea(contours[i]);
      
          }
      }

      


//-----------------------------------------trackbar eliminate small or undesired region manually--------------------------------- 


      namedWindow("contour",WINDOW_NORMAL); resizeWindow("contour",600,600);
      int cont_size=0;


      createTrackbar("Contour Threshold","contour",&cont_size,maxarea_thresh);              
      
      while(1)
      {
        drawing =all_contour.clone();
        masked=src.clone();

       for( size_t i = 0; i< contours.size(); i++ )                                         //draws contour which are required only
      
      {
        
        if (contourArea(contours[i]) > cont_size)
        {
          drawContours( drawing, contours, (int)i,Scalar(0,0,255),CV_FILLED);
          drawContours( masked, contours, (int)i,Scalar(0,0,255));
        }
      }
      imshow("contour",masked);

        char a = waitKey(33);
        if(a=='c' || a=='C')
        {
          destroyWindow("contour"); 
          break;
        }
    	    
      }   



//--------------------------trackbar to select big particles whose detailed properties is fond with numbering--------------------


      namedWindow("Select Area",WINDOW_NORMAL); resizeWindow("Select Area",600,600);

      createTrackbar("Area Threshold","Select Area",&areathresh, maxarea_thresh);

      Mat image=src.clone();

      imshow("Select Area",image);

      masked3=drawing.clone();

      while(1)
      {

        drawing=masked3.clone();
        
        for( size_t i = 0; i< contours.size(); i++ )
        {

          if (contourArea(contours[i])>areathresh && contourArea(contours[i]) > cont_size)
          {

           drawContours( image, contours, (int)i, Scalar(255,0,0),CV_FILLED );
           drawContours( drawing, contours, (int)i, Scalar(255,0,0),CV_FILLED );

          }

        }
        
        imshow("Select Area",image);
         
        image=src.clone();


        char a = waitKey(33);
        if(a=='c' || a=='C')
        {
           
          
        c=0;                                                          //counter variable to print contour number for big particles 

        namedWindow("Final Result",WINDOW_NORMAL); resizeWindow("Final Result",600,600);

        Mat image1=src.clone();
        for( size_t i = 0; i< contours.size(); i++ )
        {

          if (contourArea(contours[i]) > cont_size)
          {

            if (contourArea(contours[i])<areathresh )
            {
            double length=max(boundingRect( contours_poly1[i] ).width,boundingRect( contours_poly1[i] ).height);
            cout<<"length :  "<<length<<"    aspect ratio:    "<<((double)min(boundingRect( contours_poly1[i] ).width,boundingRect( contours_poly1[i] ).height)/max(boundingRect( contours_poly1[i] ).width,boundingRect( contours_poly1[i] ).height))<<endl;
            }
            else
            {
              c++;

              str1<<c;
                string str2= str1.str();

                putText(image1 , str2 , centers1[i], FONT_HERSHEY_PLAIN, 1,  Scalar(0,255,255), 2 ,8, 0); 

                str1.str("");
                str1.clear();

                area_array[c-1]=contourArea(contours[i]);
                ratio[(c-1)]=(double)min(boundingRect( contours_poly1[i] ).width,boundingRect( contours_poly1[i] ).height)/max(boundingRect( contours_poly1[i] ).width,boundingRect( contours_poly1[i] ).height);
                radius[(c-1)]=radius1[i];

            }

          }

        }



        cout<<"-----------------------------------------------------------------------------------------------------------------"<<endl;
        
        imshow("Final Result",image1);
        
          for (int i = 0; i < c; ++i)
          {
        
            cout<<"count no:    "<<i+1<<"      aspect_ratio    :    " << ratio[i] <<"       diameter :    "<<(2*radius[i])<<"          Area :   "<<area_array[i]<< endl;
            cout<<"-----------------------------------------------------------------------------------------------------------------"<<endl;
        
          }
          
          break;

        }       
		
    }

    int a=0;

    Mat black_region= Mat( src.size(), CV_8UC1 );

    namedWindow("black_region",WINDOW_NORMAL); resizeWindow("black_region",600,600);

    //dilate( drawing,drawing, element );

    //-------------------------------------------------DRAW BLACK REGION--------------------------------------------------------

    createTrackbar("Black region Threshold","black_region",&B_thresh, max_thresh);

    while(1)
    {
       
        black_region=makebinary(gray,B_thresh);
        
        imshow("black_region",black_region);
        char a = waitKey(33);
        if(a=='c' || a=='C')
        {
          destroyWindow("black_region"); 
          break;
        }
            
      }
    findContours( black_region, contours, RETR_CCOMP, CHAIN_APPROX_SIMPLE );
    for( size_t i = 0; i< contours.size(); i++ )
      {
        Scalar color = Scalar( rng.uniform(0, 256), rng.uniform(0,256), rng.uniform(0,256) );
        drawContours( drawing, contours, (int)i, Scalar(0,0,0),CV_FILLED );
        }
    a=countNonZero( black_region );

namedWindow("DRAWING",WINDOW_NORMAL);
      
      resizeWindow("DRAWING",800,800);
    cout<<"Black region area :  "<<a <<" sq. pixels"<<endl;
    imshow("DRAWING",drawing); 	
    

		  waitKey(0);
    	return 0;
}




//-----------------------------------------------END OF PROGRAM :) --------------------------------------------------------------
