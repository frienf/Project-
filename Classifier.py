#This code is referred from https://blog.tensorflow.org/2019/11/how-to-get-started-with-machine.html 
# and https://eloquentarduino.github.io/2019/11/how-to-train-a-classifier-in-scikit-learn/

import numpy as np
from glob import glob
from os.path import basename
from sklearn.ensemble import RandomForestClassifier
from sklearn.model_selection import train_test_split
from micromlgen import port
from sklearn.metrics import accuracy_score, confusion_matrix, classification_report
from sklearn.preprocessing import StandardScaler

fileNames = ["Test_Gesture/A.csv","Test_Gesture/B.csv","Test_Gesture/C.csv","Test_Gesture/D.csv","Test_Gesture/E.csv","Test_Gesture/F.csv","Test_Gesture/G.csv","Test_Gesture/H.csv","Test_Gesture/I.csv","Test_Gesture/J.csv","Test_Gesture/K.csv","Test_Gesture/L.csv","Test_Gesture/M.csv","Test_Gesture/N.csv","Test_Gesture/O.csv","Test_Gesture/P.csv","Test_Gesture/V.csv"]

def load_features(files):
    dataset = None
    classmap = {}
    for class_idx, filename in enumerate(files):
        class_name = basename(filename)[:-4]
        classmap[class_idx] = class_name
        samples = np.genfromtxt(filename,delimiter=',',filling_values=0.0)
        labels = np.ones((len(samples), 1)) * class_idx
        samples = np.hstack((samples, labels))
        dataset = samples if dataset is None else np.vstack((dataset, samples))

    return dataset, classmap

if __name__ == '__main__':
    features, classmap = load_features(fileNames)
    X, y = features[:, :-1], features[:, -1]
    x_train, x_test, y_train, y_test= train_test_split(X, y, test_size= 0.25, random_state=0)
    model=RandomForestClassifier(n_estimators=20, max_depth=10).fit(x_train,y_train)
    #st_x= StandardScaler()    
    #x_train= st_x.fit_transform(x_train)    
    #x_test= st_x.transform(x_test)
    print('Score on train set: %.2f',model.score(x_train,y_train))
    print('Score on test set: %.2f',model.score(x_test,y_test))
    y_predict=model.predict(x_test)
    print(accuracy_score(y_test, y_predict))
    #print(classification_report(y_test, y_predict))
    cn=confusion_matrix(y_test,y_predict)
    print(cn)
    print("Writing to a file")
    c_code = port(model, classmap=classmap)
    modelFile = open("model.h", "w")
    modelFile.write(c_code)
    modelFile.close()

    print("Model file created")
