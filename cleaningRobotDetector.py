import cv2
import numpy as np
import paho.mqtt.client as mqtt

url='http://192.168.1.102:81/stream'
cap = cv2.VideoCapture(url)

mqttBroker = "broker.emqx.io"
client = mqtt.Client("Cleaning-Robot",clean_session=True,userdata=None)
client.connect(mqttBroker,1883)


temp=4

while True:

    _, frame = cap.read()
    frame = cv2.flip(frame, 1)
    frame = cv2.resize(frame, (640, 320))
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    green_lower = np.array([33, 80, 40], np.uint8)
    green_upper = np.array([85, 255, 255], np.uint8)
    mask = cv2.inRange(hsv, green_lower, green_upper)
    contours, _ = cv2.findContours(mask, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    contours = sorted(contours,key=lambda x:cv2.contourArea(x),reverse=True)
    rows, cols, _ = frame.shape
    center_x = int(rows / 2)
    center_y = int(cols / 2)

    for cnt in contours:
        (x, y, w, h) = cv2.boundingRect(cnt)
        cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)
        #////////////////////////////////////////////////////////////////////
        medium_x = int((x + x+w)/2)
        medium_y = int((y + y+h)/2)
        #////////////////////////////////////////////////////////////////////
        cv2.line(frame, (medium_x,0),(medium_x,600),(0,255,0),2)
        text2 = "mediumX = " + str(medium_x)
        cv2.putText(frame, text2, (0, 100), cv2.FONT_HERSHEY_SIMPLEX, 0.50, (0, 255, 50))
        cv2.line(frame, (0, medium_y), (600, medium_y), (0, 255, 0), 2)




        if (medium_x < 220 and temp != 1):
            temp = 1
            client.publish("POSITION","RIGHT")
            print("Just published " + "RIGHT" + " to Topic POSITION")



        elif (220 < medium_x < 380 and temp != 2):
            temp = 2

            client.publish("POSITION", "FRONT")
            client.publish("POSITION", "STOP")
            print("Just published " + "FORWARD" + " to Topic POSITION")



        elif (medium_x > 380 and temp != 3):

            temp = 3
            client.publish("POSITION", "LEFT")
            print("Just published " + "LEFT" + " to Topic POSITION")

        break


    cv2.imshow("frame",frame)
    key = cv2.waitKey(1)
    if key ==27:
        break
cap.release()
cv2.destroyAllWindows()