import sqlite3
import paho.mqtt.client as mqtt
from database import data

mqtt_broker = 'broker.emqx.io'
mqtt_port = 1883
mqtt_topic = 'speech'
mqtt_user = 'emqx'
mqtt_password = 'public'

# database = 'tts_mqtt.db'
database = 'db.sqlite3'

# table = 'textdata'
table = 'tts_prompt'



conn = sqlite3.connect(database)
cursor = conn.cursor()


def read_text(id):
    cursor.execute('SELECT text FROM {} WHERE id = ?'.format(table), (id,))
    result = cursor.fetchone()
    
    if result is not None:
        text = result[0]
        mqtt_client.publish(mqtt_topic, str(text))
        print(text)
    else:
        mqtt_client.publish(mqtt_topic, 'invalidId')


def on_connect(client, userdata, flags, rc):
    print("hii")
    print('connect to code' + str(rc))
    # client.subscribe(mqtt_topic)
    number = len(data)
    print(number)
    for index in range(number):
        read_text(index)


# def on_message(client, userdata, msg):
#     number = len(data)
#     print(number)
#     for index in range(number):
#         read_text(index)


mqtt_client = mqtt.Client()


mqtt_client.on_connect = on_connect
# mqtt_client.on_message = on_message

mqtt_client.connect(mqtt_broker, mqtt_port, 60)


mqtt_client.loop_forever()