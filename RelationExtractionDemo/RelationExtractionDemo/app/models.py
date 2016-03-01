"""
Definition of models.
"""

from django.db import models

# Create your models here.
class Wikiinfotable(models.Model):
    entity1 = models.CharField(max_length=100)
    relationtype = models.CharField(max_length=100)
    entity2 = models.CharField(max_length=1000)
