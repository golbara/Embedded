from django.db import models
from django.forms import ModelForm
# Create your models here.
class prompt(models.Model):
    text = models.TextField()
    def __str__(self):
        return self.text
    
class promptForm(ModelForm):
    class Meta:
        model = prompt
        fields = ["text"]
