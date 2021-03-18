INSERT INTO
    driver (first_name, last_name, email, phone_number, msr_id, scca_id, work_req, work_skill)
  VALUES
    ('Nick', 'Maludy', 'fake@gmail.com', '1234', 1, 2, NULL, NULL);


INSERT INTO
    driver (first_name, last_name, email, phone_number, msr_id, scca_id, work_req, work_skill)
  VALUES
    ('Brad', 'Bishop', 'brad@gmail.com', '4321', 2, 3, NULL, NULL);


INSERT INTO
    driver (first_name, last_name, email, phone_number, msr_id, scca_id, work_req, work_skill)
  VALUES
    ('Nick', 'Maludy', 'fake@gmail.com', '1234', 1, 2, NULL, NULL),
    ('Brad', 'Bishop', 'brad@gmail.com', '4321', 2, 3, NULL, NULL);



UPDATE driver SET (first_name, last_name) = ('test', 'test2') WHERE driver_id = 1;