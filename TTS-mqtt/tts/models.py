from django.db import models

# Create your models here.
class prompt(models.Model):
    text = models.TextField()