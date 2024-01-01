from django.shortcuts import render
from .models import *
from .serializers import PromptSerializer
from django.http import Http404
from rest_framework.views import APIView
from rest_framework.response import Response
from rest_framework import status


class promptAPIView(APIView):
    """
    List all snippets, or create a new snippet.
    """
    def get(self, request, format=None):
        prompts = prompt.objects.all()
        serializer = PromptSerializer(prompts, many=True)
        return Response(serializer.data)

    def post(self, request, format=None):
        serializer = PromptSerializer(data=request.data)
        if serializer.is_valid():
            serializer.save()
            return Response(serializer.data, status=status.HTTP_201_CREATED)
        return Response(serializer.errors, status=status.HTTP_400_BAD_REQUEST)
# Create your views here.
