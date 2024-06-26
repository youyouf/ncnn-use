import torch
import torchvision.models as models
import pnnx

model = models.resnet18(pretrained=True)

x = torch.rand(1, 3, 224, 224)

opt_model = pnnx.export(model, "../models/resnet18.pt", x)