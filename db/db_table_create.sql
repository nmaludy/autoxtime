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
  phone_number TEXT NOT NULL,
  msr_id INT NOT NULL,
  scca_id INT NOT NULL,
  work_req INT,
  work_skill INT,
  PRIMARY KEY(driver_id),
  CONSTRAINT fk_work_assignments
    FOREIGN KEY(work_req)
      REFERENCES work_assignments(work_id),
  CONSTRAINT fk_work_assignments_skill
    FOREIGN KEY(work_skill)
      REFERENCES work_assignments(work_id)
);

CREATE TABLE car_class(
  car_class_id INT GENERATED ALWAYS AS IDENTITY,
  name TEXT NOT NULL,
  region_id INT NOT NULL,
  pax_formula TEXT,
  PRIMARY KEY(car_class_id),
  CONSTRAINT fk_region
    FOREIGN KEY(region_id)
      REFERENCES region(region_id)
);

CREATE TABLE car(
  car_id INT GENERATED ALWAYS AS IDENTITY,
  car_number INT NOT NULL,
  year INT NOT NULL,
  make TEXT NOT NULL,
  model TEXT NOT NULL,
  color TEXT NOT NULL,
  sponsor TEXT NOT NULL,
  tire_brand TEXT NOT NULL,
  car_class_id INT NOT NULL,
  driver_id INT NOT NULL,
  region_id INT NOT NULL,
  PRIMARY KEY(car_id),
  CONSTRAINT fk_car_class
    FOREIGN KEY(car_class_id)
      REFERENCES car_class(car_class_id),
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

CREATE TABLE penalties(
  penalty INT,
  raw_id INT,
  run_id INT,
  CONSTRAINT fk_raw_times
    FOREIGN KEY(raw_id)
      REFERENCES raw_times(raw_id),
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

CREATE TABLE users(
  user_id INT GENERATED ALWAYS AS IDENTITY,
  username TEXT NOT NULL,
  password TEXT NOT NULL,
  driver_id INT,
  active INT NOT NULL,
  PRIMARY KEY(user_id),
  CONSTRAINT fk_driver
    FOREIGN KEY(driver_id)
      REFERENCES driver(driver_id)
);

CREATE TABLE role (
  role_id INT GENERATED ALWAYS AS IDENTITY,
  role TEXT NOT NULL,
  PRIMARY KEY (role_id)
);

CREATE TABLE user_role (
  user_id INT,
  role_id INT,
  PRIMARY KEY (user_id, role_id),
  CONSTRAINT fk_users
    FOREIGN KEY(user_id)
      REFERENCES users(user_id),
  CONSTRAINT fk_role
    FOREIGN KEY(role_id)
      REFERENCES role(role_id)
);

INSERT INTO role (role) VALUES ('user_read');
INSERT INTO role (role) VALUES ('user_write');
INSERT INTO role (role) VALUES ('admin');
