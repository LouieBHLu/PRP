import cv2
import numpy as np

picture=cv2.imread('C:/Users/86181/Desktop/opencv/test.jpg')
cv2.namedWindow('test',cv2.WINDOW_AUTOSIZE)
cv2.imshow('test',picture)
print(picture.shape)                                             #高，宽，通道数（bgr）
print(picture.size)                                              #图像大小
print(picture.dtype)                                             #数据类型
print(np.array(picture))                                         #计算机眼里的图像
cv2.waitKey(0)
cv2.destroyAllWindows()