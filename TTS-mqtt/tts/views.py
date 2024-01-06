from django.shortcuts import render
from .models import prompt
from .serializers import PromptSerializer
from .forms import promptForm  # Import your form
from django.http import Http404
from rest_framework.views import APIView
from rest_framework.response import Response
from rest_framework import status
import sqlite3
import paho.mqtt.client as mqtt
from database import data

mqtt_broker = 'broker.emqx.io'
mqtt_port = 1883
mqtt_topic = 'speech'
mqtt_user = 'emqx'
mqtt_password = 'public'

database = 'db.sqlite3'
table = 'tts_prompt'

conn = sqlite3.connect(database)
cursor = conn.cursor()

mqtt_client = mqtt.Client()

def on_connect(client, userdata, flags, rc):
    print("hii")
    print('connect to code' + str(rc))
    number = len(data)
    print(number)
    for index in range(number):
        read_text(index)

mqtt_client.on_connect = on_connect
mqtt_client.connect(mqtt_broker, mqtt_port, 60)

def read_text(id):
    cursor.execute('SELECT text FROM {} WHERE id = ?'.format(table), (id,))
    result = cursor.fetchone()
    
    if result is not None:
        text = result[0]
        mqtt_client.publish(mqtt_topic, str(text))
        print(text)
    else:
        mqtt_client.publish(mqtt_topic, 'invalidId')


# promptAPIView
from django.shortcuts import render, redirect
from django.urls import reverse

class promptAPIView(APIView):
    def get(self, request):
        form = promptForm()  # Create an instance of the form
        return render(request, 'form.html', {'form': form})

    def post(self, request, format=None):
        serializer = PromptSerializer(data=request.data)
        if serializer.is_valid():
            serializer.save()
            mqtt_client.publish(mqtt_topic, serializer.data['text'])  # Use serializer.data
            print("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&7")
            print("sent")
            # Redirect to the same page with a success query parameter
            return redirect(reverse('tts:prompt') + '?success=true')

        return render(request, 'form.html', {'form': promptForm(), 'error': 'Invalid form submission'})
