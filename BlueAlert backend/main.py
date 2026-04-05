from fastapi import FastAPI, HTTPException, Depends
from pydantic import BaseModel, Field
from typing import List, Annotated
import models
from database import session_local, engine
from sqlalchemy.orm import Session
from datetime import date

app = FastAPI()
models.Base.metadata.create_all(bind=engine)

# ------------------- DB Dependency -------------------
def get_db():
    db = session_local()
    try:
        yield db
    finally:
        db.close()

db_dependency = Annotated[Session, Depends(get_db)]

# ------------------- Schemas -------------------
class DiseaseRecord(BaseModel):
    name: str
    description: str

class DiseaseResponse(DiseaseRecord):
    disease_id: int
    class Config:
        from_attributes = True

class LocationRecord(BaseModel):
    name: str
    region: str

class LocationResponse(LocationRecord):
    location_id: int
    class Config:
        from_attributes = True

class ReporterRecord(BaseModel):
    name: str
    contact: str = Field(min_length=10, max_length=10)

class ReporterResponse(ReporterRecord):
    reporter_id: int
    class Config:
        from_attributes = True

class DeviceRecord(BaseModel):
    status: bool
    install_location: str

class DeviceResponse(DeviceRecord):
    device_id: int
    class Config:
        from_attributes = True   

class AlertRecord(BaseModel):
    Alert_id: int
    disease_id: int
    location_id: int
    case_count: int
    class Config:
        from_attributes = True

class ReportsRecord(BaseModel):
    disease_id: int
    location_id: int
    reporter_id: int
    report_date: date

class ReportsResponse(ReportsRecord):
    report_id: int
    class Config:
        from_attributes = True 

class DeviceDataRecord(BaseModel):
    device_id: int
    id: int
    date: str
    temp_data: float
    turbidity_data: float
    conductivity_data: int

    class Config:
        from_attributes = True

# ------------------- Diseases -------------------
@app.post("/diseases", response_model=DiseaseResponse)
def post_disease(data: DiseaseRecord, db: db_dependency):
    item = models.diseaseTable(name=data.name, description=data.description)
    db.add(item)
    db.commit()
    db.refresh(item)
    return item

@app.get("/diseases", response_model=List[DiseaseResponse])
def get_disease(db: db_dependency):
    return db.query(models.diseaseTable).all()

# ------------------- Locations -------------------
@app.post("/locations", response_model=LocationResponse)
def post_location(data: LocationRecord, db: db_dependency):
    item = models.locationTable(name=data.name, description=data.region)
    db.add(item)
    db.commit()
    db.refresh(item)
    return item

@app.get("/locations", response_model=List[LocationResponse])
def get_location(db: db_dependency):
    return db.query(models.locationTable).all()

# ------------------- Reporters -------------------
@app.post("/reporters", response_model=ReporterResponse)
def post_reporter(data: ReporterRecord, db: db_dependency):
    item = models.reporterTable(name=data.name, description=data.contact)
    db.add(item)
    db.commit()
    db.refresh(item)
    return item

@app.get("/reporters", response_model=List[ReporterResponse])
def get_reporter(db: db_dependency):
    return db.query(models.reporterTable).all()

# ------------------- Devices -------------------
@app.post("/devices", response_model=DeviceResponse)
def post_device(data: DeviceRecord, db: db_dependency):
    item = models.deviceTable(active=data.status, install_location=data.install_location)
    db.add(item)
    db.commit()
    db.refresh(item)
    return item

@app.get("/devices", response_model=List[DeviceResponse])
def get_device(db: db_dependency):
    return db.query(models.deviceTable).all()

# ------------------- Alerts -------------------
@app.get("/alerts", response_model=List[AlertRecord])
def get_alert(db: db_dependency):
    return db.query(models.alertTable).all()

# ------------------- Reports -------------------
@app.post("/reports", response_model=ReportsResponse)
def post_diseaseReport(data: ReportsRecord, db: db_dependency):
    item = models.diseaseReportsTable(
        disease=data.disease_id,
        location=data.location_id,
        reporter=data.reporter_id,
        date=data.report_date
    )
    db.add(item)
    db.commit()
    db.refresh(item)
    return item

@app.get("/reports", response_model=List[ReportsResponse])
def get_report(db: db_dependency):
    return db.query(models.diseaseReportsTable).all()

# ------------------- Device Data -------------------
@app.post("/device_data", response_model=DeviceDataRecord)
def post_device_data(data: DeviceDataRecord, db: db_dependency):
    item = models.deviceDataTable(
        device_id=data.device_id,
        date=data.date,
        temp_data=data.temp_data,
        turbidity_data=data.turbidity_data,
        conductivity_data=data.conductivity_data
    )
    db.add(item)
    db.commit()
    db.refresh(item)
    return item

@app.get("/device_data", response_model=DeviceDataRecord)
def post_device_data(db: db_dependency):
    today = date.today()
    return db.query(models.deviceDataTable).filter(models.deviceDataTable.date == today).first()