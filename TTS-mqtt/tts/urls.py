from django.urls import path, include
from .views import *
app_name = 'tts'  # Make sure you define the app_name

urlpatterns = [

    path('send/', promptAPIView.as_view(), name='prompt'),
]


