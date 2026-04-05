from sqlalchemy import Column, Integer, String, Text, Date, ForeignKey, Boolean, Float
from database import Base
from datetime import datetime


class diseaseTable(Base):
    __tablename__= 'Diseases'
    disease_id = Column(Integer, primary_key=True, autoincrement=True)
    name = Column(String(20), nullable = False)
    description = Column(Text, nullable = True)

class locationTable(Base):
    __tablename__='Locations'
    location_id = Column(Integer, primary_key=True, autoincrement=True)
    name = Column(String(20), nullable= False)
    region = Column(String(20), nullable = True)

class reporterTable(Base):
    __tablename__='Reporters'
    reporter_id = Column(Integer, primary_key=True, autoincrement=True)
    name = Column(String, nullable=False)
    contact = Column(String(10), nullable=False)

class diseaseReportsTable(Base):
    __tablename__='Reports'
    report_id = Column(Integer, primary_key=True, autoincrement=True)
    disease_id = Column(Integer, ForeignKey('Diseases.disease_id'))
    location_id = Column(Integer, ForeignKey('Locations.location_id'))
    reporter_id = Column(Integer, ForeignKey('Reporters.reporter_id'))
    report_date = Column(Date, nullable= False)
    case_count = Column(Integer, nullable= False)

class alertTable(Base):
    __tablename__='Alerts'
    alert_id = Column(Integer, primary_key=True, autoincrement=True)
    disease_id = Column(Integer, ForeignKey('Diseases.disease_id'))
    location_id = Column(Integer, ForeignKey('Locations.location_id'))
    case_count = Column(Integer, nullable= True)

class deviceTable(Base):
    __tablename__='Devices'
    device_id = Column(Integer, primary_key=True)
    active = Column(Boolean, default=False)
    install_location = Column(Integer, ForeignKey('Locations.location_id'), nullable=False)

class deviceDataTable(Base):
#     __tablename__='Devices_data'
#     data_id = Column(Integer, primary_key=True, autoincrement=True)
#     device_id = Column(Integer, ForeignKey('Devices.device_id'), primary_key=True)
#     #date = Column(String, primary_key=True, default="16-09-2025")
#     date = Column(String, primary_key=True, default="12:00:00")
#     pH_data = Column(Integer, nullable= True)
#     ORP_data = Column(Integer, nullable= True)
#     temp_data = Column(Float, nullable= True)
#     turbidity_data = Column(Float, nullable= True)
#     conductivity_data = Column(Integer, nullable= True)

    __tablename__ = "Devices_data"
    data_id = Column(Integer, primary_key=True, index=True, autoincrement=True)
    device_id = Column(Integer, nullable=False)
    date = Column(Date, nullable=False)
    pH_data = Column(Float, nullable=True)
    ORP_data = Column(Float, nullable=True)
    temp_data = Column(Float, nullable=False)
    turbidity_data = Column(Float, nullable=False)
    conductivity_data = Column(Float, nullable=False)