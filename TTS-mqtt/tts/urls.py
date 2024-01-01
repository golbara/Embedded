from django.urls import path, include
from .views import *


urlpatterns = [

    path('send/', promptAPIView.as_view(), name='prompt'),
]


