// LabelingSample.cpp 
#include <sys/time.h>
#include "Labeling.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

// 適当な色を出す用
cv::RNG rnd(1192);
cv::Scalar randomColor()
{
  return cv::Scalar(rnd.next() & 0xFF, rnd.next() & 0xFF, rnd.next() & 0xFF);
}

int labelingh()
{
  struct timeval s,e;

  gettimeofday(&s,NULL);
  // 入力画像を読み込み
  cv::Mat img = cv::imread("img_0.png");
	
  // グレイスケール化(3チャンネル→1チャンネル化)
  cv::Mat gryimg;
  cv::cvtColor(img, gryimg, CV_BGR2GRAY);
  // 二値化
  cv::Mat binimg;
  cv::threshold(gryimg, binimg, 0, 255, CV_THRESH_BINARY);

  // Lblingの結果を受け取る
  cv::Mat lbl(img.size(), CV_16SC1);

  // ラベリングを実施 ２値化した画像に対して実行する
  LabelingBS	lbling;
  lbling.Exec(binimg.data, (short *)lbl.data, img.cols, img.rows, false, 0);	
	
  // ラベリング結果を出力する、真っ白な状態で初期化
  cv::Mat outimg(img.size(), CV_8UC3, cv::Scalar(0));
	
  // ラベルされた領域をひとつずつ描画
  for( int i = 0; i < lbling.GetNumOfRegions(); i++)
    {
      // ラベリング結果の領域を抽出する
      cv::Mat lblarea;
      cv::compare(lbl, i + 1, lblarea, CV_CMP_EQ);
      // 抽出した領域にランダムな色を設定して出力画像に追加
      cv::Mat color(img.size(), CV_8UC3, randomColor());
      color.copyTo(outimg, lblarea);
    }

  // ラベルされた出力画像を保存
  cv::imwrite("result_img_0.png", outimg);
  
  gettimeofday(&e,NULL);

  //時間を計測
  std::cout << "time =" << (e.tv_sec - s.tv_sec) + (e.tv_usec - s.tv_usec ) / 100000.0 << std::endl;

  return 0;
}
