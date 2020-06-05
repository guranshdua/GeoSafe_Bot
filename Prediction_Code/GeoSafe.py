import numpy as np
import matplotlib.pyplot as plt
import pandas as pd

dataset = pd.read_csv('tarp-data.csv')

X = dataset.iloc[:, 1:5].values
y = dataset.iloc[:, 5].values

from sklearn.model_selection import train_test_split
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size = 0.3, random_state = 0)


from sklearn.preprocessing import StandardScaler
sc = StandardScaler()
X_train = sc.fit_transform(X_train)
X_test = sc.transform(X_test)


import keras
from keras.models import Sequential
from keras.layers import Dense


classifier = Sequential()


classifier.add(Dense(units = 6, kernel_initializer = 'uniform', activation = 'relu', input_dim = 4))


classifier.add(Dense(units = 6, kernel_initializer = 'uniform', activation = 'relu'))


classifier.add(Dense(units = 1, kernel_initializer = 'uniform', activation = 'sigmoid'))

classifier.compile(optimizer = 'adam', loss = 'binary_crossentropy', metrics = ['accuracy'])


classifier.fit(X_train, y_train, batch_size = 40, epochs = 150)

"""
data = [70,200,50,80]
arr=[]
arr.append(data) 
df = pd.DataFrame(arr, columns = ['HUMID', 'MQ135','MQ2','temp'])
y_predcheck = classifier.predict(df)
y_predcheck = ((y_predcheck > 0.5))
"""

y_pred = classifier.predict(X_test)
y_pred = ((y_pred > 0.5))

from sklearn.metrics import confusion_matrix
cm = confusion_matrix(y_test, y_pred)

from sklearn.metrics import accuracy_score

accuracy_score(y_test, y_pred)