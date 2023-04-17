#include "worker.h"


// Declare stable frame(s) here
// static uchar pix[W*H];

//ui->image->setPixMap(QPixmap::fromImage(pImageBuffer))


//LARACATU
// Namespace for using pylon objects.
using namespace Pylon;
// Namespace for using cout.
using namespace std;
//LARACATUFIN
using namespace cv;





Worker::Worker(QObject *parent) : QThread(parent)
{
    // Init RNG
     RandGen=std::mt19937(rd());
     RandDist=std::uniform_int_distribution<int>(0,99);

     // Init (and configure) cameras here
     total=0;
     for(int i=0;i<20;i++){
         frames_counter[i]=-1;
     }
     HaveCams=InitCams();


}


Worker::~Worker()
{
    // In (strange) case we are closing the shop with camera still streaming
    GoOn=0;
    // May wait here for a few ms, to give time for cam to stop grabbing,
    // so freeing resources will not cause an error on exit

    // Release any resources here

    // Close cameras here

    // if(HaveCams) ...
}

void Worker::run()
{
    // StartStreaming here
    StartStreamin();
    switch(Mode) {
        case 0:
            break;
        case 1:
            DoDaWork();
            break;
    }

    // StopStreaming here

    GoOn=0;
}






void Worker::DoDaWork()
{

    nhits=0;

    try {

        // Declare grab result here
        CGrabResultPtr ptrGrabResult;
        while(GoOn && camera.IsGrabbing()) {

            // Wait for all cameras here
            camera.RetrieveResult( 5000, ptrGrabResult, TimeoutHandling_ThrowException );
            int co=0;
            while(!ptrGrabResult->GrabSucceeded()) {
                // Random gen used here to simulate grab wait and timeouts
                // In actual application, wait for grab success instead
                msleep(50);
                if(!GoOn) {return;}
                if(++co>59) {emit TimeOut(); co=0;}
            }

            // For all cameras:
                // Get frame(s) here from grab result
            const uint8_t* pImageBuffer = (uint8_t*) ptrGrabResult->GetBuffer();

                // Copy frame(s) to stable buffer
            memcpy(pix,pImageBuffer,WW*HH);


                // Re-enqueue frame(s) for next grab

            // Process stable frame(s)
            ArgollaCount();
            // Pass to interface whatever results or indicators to show in real time
            emit Result(++nhits);

        }

    } catch(...) {
        // Use Kaboum to signal the interface the type of problem that happened
        Kaboum=1;
        GoOn=0;
        return;
    }
}



// ------------------- Init camera(s) and grabber(s) -------------------------
bool Worker::InitCams(void)
{
    PylonInitialize();
    try {

        // Do whatever to initialize connection
        camera.Attach( CTlFactory::GetInstance().CreateFirstDevice() );
        cout << "Using camera: " << camera.GetDeviceInfo().GetModelName() << endl;
        camera.MaxNumBuffer = 5;


        // Sometimes it's convenient to wait a little for the hardware to warm up
        // before connection can be established, so we wait and retry a few times

        int c=0;  // Wait for cam máx 30 secs.
patience:
        try {
            while(false) {//CameraNotResponding
                std::this_thread::sleep_for(std::chrono::seconds(1));
                c++; if(c>30) return false;
            }
        } catch (const Pylon::GenericException& e) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            c++; if(c>30) return false;
            goto patience;
        } catch (...) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            c++; if(c>30) return false;
            goto patience;
        }


        // Cam successfully connected, set up the rest of things

        // Configure the cameras, if needed (normally, the proper config is already
        // saved into the cam) before starting any streaming

        // Let camera(s) ready to begin streaming

    } catch(const Pylon::GenericException& e) {
        return false;
    } catch(...) {
        return false;
    }

    return true;
}


// ------------------------------- Init grab -------------------------------
bool Worker::StartStreamin(void)
{

    try {

        // Camera(s) start streaming
        camera.StartGrabbing();
        // This smart pointer will receive the grab result data.






        // Start the grabbing of c_countOfImagesToGrab images.
        // The camera device is parameterized with a default configuration which
        // sets up free-running continuous acquisition.


    } catch(const Pylon::GenericException& e) {
        return false;
    }

    return true;
}

bool Worker::StopStreamin(void)
{
    return false;
}


void Worker::ArgollaCount(void){
    // Get frame(s) here from grab result
    //unsigned char frame[WW*HH];
    //memcpy(frame,pix,WW*HH);
    Mat frame= Mat(HH, WW, CV_8UC1, pix);

    cv::threshold(frame, frame, 125, 255, cv::THRESH_BINARY);
    list<vector<Point>> contour_circles;
    vector<vector<Point> > contours;
    vector<Vec4i> iji;
    cv::findContours(frame,contours, iji,cv::RETR_TREE ,cv::CHAIN_APPROX_SIMPLE);
    if (contours.size()==0){
        cout<<"No contours";
    }
    vector<Rect> boundRect( contours.size() );
    vector<float>radius( contours.size() );
    vector<Point2f>centers( contours.size() );
    vector<RotatedRect> minRect( contours.size() );
    vector<Point2f> convexHull;
    vector<Point2f> contourhull;
    int contador=0;
    float perimeter;
    float area;
    float circularity;
    for( size_t i = 0; i< contours.size(); i++ )
        {
        boundRect[i] = boundingRect( contours[i] );
        cv::minEnclosingCircle( contours[i], centers[i], radius[i] );
        if(radius[i]<200.0 && radius[i]>40.0){
            perimeter=cv::arcLength(contours[i],TRUE);
            area=cv::contourArea(contours[i]);

            if(perimeter==0){
            break;
            }
            circularity = 4*M_PI*(area/(perimeter*perimeter));



            contour_circles.push_back(contours[i]);
            minRect[i]=cv::minAreaRect(contours[i]);
            Point2f rect_points[4];

            minRect[i].points(rect_points);

            float diffX=(rect_points[0].x-rect_points[1].x);
            float diffY=(rect_points[0].y-rect_points[1].y);
            float a=sqrt((diffY * diffY) + (diffX * diffX));
            float diffX2=(rect_points[1].x-rect_points[2].x);
            float diffY2=(rect_points[1].y-rect_points[2].y);
            float b=sqrt((diffY2 * diffY2) + (diffX2 * diffX2));


            if(abs(a-b)>10){

                contador++;




                for ( int j = 0; j < 4; j++ )
                       {
                           line( frame, rect_points[j], rect_points[(j+1)%4], 0,5 );
                       }


            }


            //cv::drawContours( drawing, contours, (int)i, color, 2, LINE_8, hierarchy, 0 );
        }//final if radius
    }//final for
    if( check(20) && contador!=0){
        for( int i = 0; i < contador; i++ ){
            frames_counter[i]=23;
            total++;
        }
    }

    else if(frames_counter[contador-1]!=-1){
        for( int i = 0; i < contador; i++ ){
            frames_counter[i]--;
        }
        if(frames_counter[contador]==-1){
            frames_counter[contador]=23;
            total++;
        }


    }
    if(frames_counter[0]==0){
        for (int i =0; i > 20-1; i++){
                frames_counter[i] = frames_counter[i+1];
        }

    }
    memcpy(pix2,(unsigned char*)frame.data,WW*HH);



}

bool Worker::check( int n){
    for (int i = 0; i < n - 1; i++)
    {
        if (frames_counter[i] != frames_counter[i + 1])
            return true;
    }
    return false;
}


/*
// ---------------------- Release camera resources -------------------------
void Worker::CloseCams(void)
{
    try {

        // Free all resources
        PylonTerminate();
    } catch(const Pylon::GenericException& e) {
        QMessageBox msgBox;
        QString msg(e.what());
        msgBox.setText(msg);
        msgBox.exec();
    }
}

// ---------------------- Config camera -------------------------

void Worker::ConfigCams(void) {

    // Pass whatever config parameters needed by the cam(s)

    // Setting up the config with PylonWiever beforehand
    // and storing it into the cam(s) for autoload at startup
    // is recommended, except those parameters that may need
    // adjusting upon specific varying circumstances (if they
    // exist in the actual application) at startup time of every
    // session (e.g. a mobile machine used in different environments
    // of for different kind of targets every time, or e.g. a fixed
    // machine in a production line with different working conditions
    // in different sessions (belt speed, amount of light, etc.)

}

*/


