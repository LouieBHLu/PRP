import cv2

picture=cv2.imread('C:/Users/86181/Desktop/opencv/test.jpg') #路径的斜杠不要打反，路径里不要有中文
cv2.namedWindow('test',cv2.WINDOW_AUTOSIZE)                      #WINDOW_NORMAL
cv2.imshow('test',picture)
cv2.waitKey(0)                                                   #括号内的输入为毫秒数
cv2.destroyAllWindows()

capture=cv2.VideoCapture(0)                                      #0代表开启电脑摄像头，也可填路径
while(True):                                                    #做一个循环，每10ms得到一个图像
    ret,frame=capture.read()
    cv2.imshow('vedio',frame)
    c=cv2.waitKey(10)
    if c == 27:                                                  #27对应阿斯卡🐎的esc键，即按下esc退出
        break