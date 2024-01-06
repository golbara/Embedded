from rest_framework import serializers
from .models import *

class PromptSerializer(serializers.ModelSerializer):
    class Meta:
        model = prompt
        fields = '__all__'