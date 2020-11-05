import cv2
import os
import numpy as np
import matplotlib.pyplot as plt
def LDA(x_train,y_train):
    sun=[]
    huang=[]
    for i in range(len(y_train)):
        if y_train[i]==1:
            huang.append(x_train[i])
        else:
            sun.append(x_train[i])
    mu0=np.mean(np.array(huang).transpose(),axis=1)
    mu1=np.mean(np.array(sun).transpose(),axis=1)
    sigma0=np.cov(np.array(huang).transpose())
    sigma1=np.cov(np.array(sun).transpose())
    return np.linalg.inv(sigma0+sigma1)@((mu0-mu1).transpose())
#print(np.cov(np.array([[3,2,4],[4,6,5]])))
#print(np.linalg.inv(np.array([[-2,1],[4,-3]])))
#def predict(x_test):
X = np.array([[-1, -3], [-2, -1], [-3, -2], [1, 1], [2, 1], [2, 3]])
y = np.array([1, 1, 1, 2, 2, 2])
w=LDA(X,y)
print(w)
x = np.arange(-5, 5, 0.1)
f=w[1]*x/w[0]
plt.xlim(-4,4)
plt.ylim(-4,4)
plt.plot(x, f)
plt.plot([-1,-2,-3],[-3,-1,-2],'ro')
plt.plot([1,2,2],[1,1,3],'*')
plt.show()
