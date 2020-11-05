# 导入所需工具包
from pyzbar import pyzbar
import argparse
import cv2

# 构建参数解析器并解析参数
ap = argparse.ArgumentParser()
ap.add_argument("-i", "--image", required=False, help="path to input image")
args = vars(ap.parse_args())

# 加载输入图像
image1 = cv2.imread(args["image"])

# 找到图像中的条形码并进行解码
bars = pyzbar.decode(image1)

# 循环检测到的条形码
for bar in bars:
 # 提取条形码的边界框的位置
 # 画出图像中条形码的边界框
 (x, y, w, h) = bar.rect
 cv2.rectangle(image1, (x, y), (x + w, y + h), (0, 0, 255), 2)

 # 条形码数据为字节对象，所以如果我们想在输出图像上
 # 画出来，就需要先将它转换成字符串
 barcodeData = bar.data.decode("utf-8")
 barcodeType = bar.type

 # 绘出图像上条形码的数据和条形码类型
 text = "{} ({})".format(barcodeData, barcodeType)
 cv2.putText(image1, text, (x, y - 10), cv2.FONT_HERSHEY_SIMPLEX,
  0.5, (0, 0, 255), 2)

 # 向终端打印条形码数据和条形码类型
 print("[INFO] Found {} barcode: {}".format(barcodeType, barcodeData))

# 展示输出图像
cv2.imshow("Image", image1)
cv2.waitKey(0)