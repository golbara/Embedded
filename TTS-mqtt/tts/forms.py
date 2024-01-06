from django.forms import ModelForm
from .models import prompt
class promptForm(ModelForm):
    class Meta:
        model = prompt
        fields = ["text"]
