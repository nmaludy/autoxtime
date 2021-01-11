CREATE DATABASE autoxtime;

CREATE TABLE region(
  region_id INT GENERATED ALWAYS AS IDENTITY,
  name TEXT NOT NULL,
  PRIMARY KEY(region_id)
);

CREATE TABLE work_assignments(
  work_id INT GENERATED ALWAYS AS IDENTITY,
  name TEXT NOT NULL,
  PRIMARY KEY(work_id)
);

CREATE TABLE driver(
  driver_id INT GENERATED ALWAYS AS IDENTITY,
  first_name TEXT NOT NULL,
  last_name TEXT NOT NULL,
  email TEXT NOT NULL,
  msr_id INT NOT NULL,
  scca_id INT NOT NULL,
  workreq INT,
  workskill INT,
  PRIMARY KEY(driver_id),
  CONSTRAINT fk_work_assignments
    FOREIGN KEY(workreq)
      REFERENCES work_assignments(work_id),
  CONSTRAINT fk_work_assignments_skill
    FOREIGN KEY(workskill)
      REFERENCES work_assignments(work_id)
);

CREATE TABLE class(
  class_id INT GENERATED ALWAYS AS IDENTITY,
  name TEXT NOT NULL,
  region_id INT NOT NULL,
  pax_formula TEXT,
  PRIMARY KEY(class_id),
  CONSTRAINT fk_region
    FOREIGN KEY(region_id)
      REFERENCES region(region_id)
);

CREATE TABLE car(
  car_id INT GENERATED ALWAYS AS IDENTITY,
  number INT NOT NULL,
  year INT NOT NULL,
  make TEXT NOT NULL,
  model TEXT NOT NULL,
  trim TEXT,
  class_id INT NOT NULL,
  driver_id INT NOT NULL,
  region_id INT NOT NULL,
  PRIMARY KEY(car_id),
  CONSTRAINT fk_class
    FOREIGN KEY(class_id)
      REFERENCES class(class_id),
  CONSTRAINT fk_driver
    FOREIGN KEY(driver_id)
      REFERENCES driver(driver_id),
  CONSTRAINT fk_region
    FOREIGN KEY(region_id)
      REFERENCES region(region_id)
);

CREATE TABLE event(
  event_id INT GENERATED ALWAYS AS IDENTITY,
  name TEXT NOT NULL,
  date date NOT NULL,
  region_id INT NOT NULL,
  PRIMARY KEY(event_id),
  CONSTRAINT fk_region
    FOREIGN KEY(region_id)
      REFERENCES region(region_id)
);

CREATE TABLE event_payments(
  amount_paid money NOT NULL,
  amount_due money NOT NULL,
  membership_type TEXT NOT NULL,
  event_id INT NOT NULL,
  driver_id INT NOT NULL,
  CONSTRAINT fk_event
    FOREIGN KEY(event_id)
      REFERENCES event(event_id),
  CONSTRAINT fk_driver
    FOREIGN KEY(driver_id)
      REFERENCES driver(driver_id)
);

CREATE TABLE run(
  run_id INT GENERATED ALWAYS AS IDENTITY,
  start_time NUMERIC,
  sector1_time NUMERIC,
  sector2_time NUMERIC,
  end_time NUMERIC,
  pax_time NUMERIC,
  dnf bool,
  scored bool,
  penalty INT,
  event_id INT NOT NULL,
  driver_id INT NOT NULL,
  car_id INT NOT NULL,
  PRIMARY KEY(run_id),
  CONSTRAINT fk_event
    FOREIGN KEY(event_id)
      REFERENCES event(event_id),
  CONSTRAINT fk_driver
    FOREIGN KEY(driver_id)
      REFERENCES driver(driver_id),
  CONSTRAINT fk_car
    FOREIGN KEY(car_id)
      REFERENCES car(car_id)
);

CREATE TABLE raw_times(
  raw_id INT GENERATED ALWAYS AS IDENTITY,
  start_time TEXT,
  sector1_time TEXT,
  sector2_time TEXT,
  end_time TEXT,
  run_id INT NOT NULL,
  PRIMARY KEY(raw_id),
  CONSTRAINT fk_run
    FOREIGN KEY(run_id)
      REFERENCES run(run_id)
);

CREATE TABLE points(
  points_id INT GENERATED ALWAYS AS IDENTITY,
  points NUMERIC,
  event_id INT NOT NULL,
  driver_id INT NOT NULL,
  car_id INT NOT NULL,
  region_id INT NOT NULL,
  PRIMARY KEY(points_id),
  CONSTRAINT fk_region
    FOREIGN KEY(region_id)
      REFERENCES region(region_id),
  CONSTRAINT fk_event
    FOREIGN KEY(event_id)
      REFERENCES event(event_id),
  CONSTRAINT fk_driver
    FOREIGN KEY(driver_id)
      REFERENCES driver(driver_id),
  CONSTRAINT fk_car
    FOREIGN KEY(car_id)
      REFERENCES car(car_id)
);



