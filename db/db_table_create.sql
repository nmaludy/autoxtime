CREATE DATABASE autoxtime;

-- Tables

CREATE TABLE IF NOT EXISTS organization(
  organization_id INT GENERATED ALWAYS AS IDENTITY,
  name TEXT NOT NULL,
  PRIMARY KEY(organization_id)
);

CREATE TABLE IF NOT EXISTS  work_assignments(
  work_id INT GENERATED ALWAYS AS IDENTITY,
  name TEXT NOT NULL,
  PRIMARY KEY(work_id)
);

CREATE TABLE IF NOT EXISTS  driver(
  driver_id INT GENERATED ALWAYS AS IDENTITY,
  first_name TEXT NOT NULL,
  last_name TEXT NOT NULL,
  email TEXT,
  phone_number TEXT,
  msr_id INT,
  scca_id INT,
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

CREATE TABLE IF NOT EXISTS car_class(
  car_class_id INT GENERATED ALWAYS AS IDENTITY,
  name TEXT NOT NULL,
  subclass_name TEXT,
  organization_id INT NOT NULL,
  pax_formula TEXT,
  PRIMARY KEY(car_class_id),
  CONSTRAINT fk_organization
    FOREIGN KEY(organization_id)
      REFERENCES organization(organization_id)
);

CREATE TABLE IF NOT EXISTS car(
  car_id INT GENERATED ALWAYS AS IDENTITY,
  car_number INT NOT NULL,
  year INT,
  make TEXT,
  model TEXT,
  color TEXT,
  sponsor TEXT,
  tire_brand TEXT,
  car_class_id INT NOT NULL,
  driver_id INT NOT NULL,
  organization_id INT NOT NULL,
  PRIMARY KEY(car_id),
  CONSTRAINT fk_car_class
    FOREIGN KEY(car_class_id)
      REFERENCES car_class(car_class_id),
  CONSTRAINT fk_driver
    FOREIGN KEY(driver_id)
      REFERENCES driver(driver_id),
  CONSTRAINT fk_organization
    FOREIGN KEY(organization_id)
      REFERENCES organization(organization_id)
);

CREATE TABLE IF NOT EXISTS season(
  season_id INT GENERATED ALWAYS AS IDENTITY,
  name TEXT NOT NULL,
  organization_id INT NOT NULL,
  PRIMARY KEY(season_id),
  CONSTRAINT fk_organization
    FOREIGN KEY(organization_id)
      REFERENCES organization(organization_id)
);

CREATE TABLE IF NOT EXISTS event(
  event_id INT GENERATED ALWAYS AS IDENTITY,
  name TEXT NOT NULL,
  date date NOT NULL,
  season_id INT NOT NULL,
  organization_id INT NOT NULL,
  PRIMARY KEY(event_id),
  CONSTRAINT fk_organization
    FOREIGN KEY(organization_id)
      REFERENCES organization(organization_id),
  CONSTRAINT fk_season
    FOREIGN KEY(season_id)
      REFERENCES season(season_id)
);


CREATE TABLE IF NOT EXISTS event_registration(
  event_registration_id INT GENERATED ALWAYS AS IDENTITY,
  amount_paid money,
  amount_due money,
  membership_type TEXT,
  event_id INT NOT NULL,
  driver_id INT NOT NULL,
  PRIMARY KEY(event_registration_id),
  CONSTRAINT fk_event
    FOREIGN KEY(event_id)
      REFERENCES event(event_id),
  CONSTRAINT fk_driver
    FOREIGN KEY(driver_id)
      REFERENCES driver(driver_id)
);

CREATE TABLE IF NOT EXISTS run(
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

CREATE TABLE IF NOT EXISTS raw_times(
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

CREATE TABLE IF NOT EXISTS penalties(
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

CREATE TABLE IF NOT EXISTS points(
  points_id INT GENERATED ALWAYS AS IDENTITY,
  points NUMERIC,
  event_id INT NOT NULL,
  driver_id INT NOT NULL,
  car_id INT NOT NULL,
  organization_id INT NOT NULL,
  PRIMARY KEY(points_id),
  CONSTRAINT fk_organization
    FOREIGN KEY(organization_id)
      REFERENCES organization(organization_id),
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

-- Functions
-- https://www.postgresql.org/docs/9.1/sql-createfunction.html

-- https://www.postgresql.org/docs/9.1/plpgsql-trigger.html
-- https://gist.github.com/colophonemes/9701b906c5be572a40a84b08f4d2fa4e
CREATE OR REPLACE FUNCTION notify_trigger() RETURNS trigger AS $trigger$
DECLARE
  rec RECORD;
  payload TEXT;
  column_name TEXT;
  column_value TEXT;
  payload_items TEXT[];
BEGIN
  -- Set record row depending on operation
  CASE TG_OP
  WHEN 'INSERT', 'UPDATE' THEN
     rec := NEW;
  WHEN 'DELETE' THEN
     rec := OLD;
  ELSE
     RAISE EXCEPTION 'Unknown TG_OP: "%". Should not occur!', TG_OP;
  END CASE;

  -- Get required fields
  FOREACH column_name IN ARRAY TG_ARGV LOOP
    EXECUTE format('SELECT $1.%I::TEXT', column_name)
    INTO column_value
    USING rec;
    payload_items := array_append(payload_items, '"' || replace(column_name, '"', '\"') || '":"' || replace(column_value, '"', '\"') || '"');
  END LOOP;

  -- Build the payload
  payload := ''
              || '{'
              || '"timestamp":"' || CURRENT_TIMESTAMP                    || '",'
              || '"operation":"' || TG_OP                                || '",'
              || '"data":{'      || array_to_string(payload_items, ',')  || '}'
              || '}';

  -- Notify the channel
  PERFORM pg_notify(TG_TABLE_NAME, payload);

  RETURN rec;
END;
$trigger$ LANGUAGE plpgsql;

-- Triggers
-- https://www.postgresql.org/docs/9.1/sql-createtrigger.html

CREATE TRIGGER notify_organization
  AFTER INSERT OR UPDATE OR DELETE
  ON organization
FOR EACH ROW EXECUTE PROCEDURE notify_trigger(
  'organization_id',
  'name'
);

CREATE TRIGGER notify_work_assignments
  AFTER INSERT OR UPDATE OR DELETE
  ON work_assignments
FOR EACH ROW EXECUTE PROCEDURE notify_trigger(
  'work_id',
  'name'
);

CREATE TRIGGER notify_driver
  AFTER INSERT OR UPDATE OR DELETE
  ON driver
FOR EACH ROW EXECUTE PROCEDURE notify_trigger(
  'driver_id',
  'first_name',
  'last_name',
  'email',
  'phone_number',
  'msr_id',
  'scca_id',
  'work_req',
  'work_skill'
);

CREATE TRIGGER notify_car_class
  AFTER INSERT OR UPDATE OR DELETE
  ON car_class
FOR EACH ROW EXECUTE PROCEDURE notify_trigger(
  'car_class_id',
  'name',
  'organization_id',
  'pax_formula'
);

CREATE TRIGGER notify_car
  AFTER INSERT OR UPDATE OR DELETE
  ON car
FOR EACH ROW EXECUTE PROCEDURE notify_trigger(
  'car_id',
  'car_number',
  'year',
  'make',
  'model',
  'color',
  'sponsor',
  'tire_brand',
  'car_class_id',
  'driver_id',
  'organization_id'
);

CREATE TRIGGER notify_season
  AFTER INSERT OR UPDATE OR DELETE
  ON season
FOR EACH ROW EXECUTE PROCEDURE notify_trigger(
  'season_id',
  'name',
  'organization_id'
);

CREATE TRIGGER notify_event
  AFTER INSERT OR UPDATE OR DELETE
  ON event
FOR EACH ROW EXECUTE PROCEDURE notify_trigger(
  'event_id',
  'name',
  'date',
  'season_id',
  'organization_id'
);

CREATE TRIGGER notify_event_registration
  AFTER INSERT OR UPDATE OR DELETE
  ON event_registration
FOR EACH ROW EXECUTE PROCEDURE notify_trigger(
  'event_registration_id',
  'amount_paid',
  'amount_due',
  'membership_type',
  'event_id',
  'driver_id'
);

CREATE TRIGGER notify_run
  AFTER INSERT OR UPDATE OR DELETE
  ON run
FOR EACH ROW EXECUTE PROCEDURE notify_trigger(
  'run_id',
  'start_time',
  'sector1_time',
  'sector2_time',
  'end_time',
  'pax_time',
  'dnf',
  'scored',
  'event_id',
  'driver_id',
  'car_id'
);

CREATE TRIGGER notify_raw_times
  AFTER INSERT OR UPDATE OR DELETE
  ON raw_times
FOR EACH ROW EXECUTE PROCEDURE notify_trigger(
  'raw_id',
  'start_time',
  'sector1_time',
  'sector2_time',
  'end_time',
  'run_id'
);

CREATE TRIGGER notify_penalties
  AFTER INSERT OR UPDATE OR DELETE
  ON penalties
FOR EACH ROW EXECUTE PROCEDURE notify_trigger(
  'penalty',
  'raw_id',
  'run_id'
);

CREATE TRIGGER notify_points
  AFTER INSERT OR UPDATE OR DELETE
  ON points
FOR EACH ROW EXECUTE PROCEDURE notify_trigger(
  'points_id',
  'points',
  'event_id',
  'driver_id',
  'car_id',
  'organization_id'
);

GRANT ALL PRIVILEGES ON DATABASE autoxtime TO autoxtime;
GRANT ALL PRIVILEGES ON ALL TABLES IN SCHEMA public TO autoxtime;
GRANT ALL PRIVILEGES ON ALL SEQUENCES IN SCHEMA public TO autoxtime;
