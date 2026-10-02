from ultralytics import YOLO

# Load a model
model = YOLO("./yolo11n.yaml") # 학습 전의 yolo 모델 구조만 가져오기

# Train the model
results = model.train(data="./mydata/mydata.yaml", epochs=100, imgsz=640, project="~/yolo/myproject")