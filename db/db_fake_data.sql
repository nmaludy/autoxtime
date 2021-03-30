INSERT INTO
    driver (first_name, last_name, email, phone_number, msr_id, scca_id, work_req, work_skill)
  VALUES
    ('Nick', 'Maludy', 'fake@gmail.com', '1234', 1, 2, NULL, NULL),
    ('Brad', 'Bishop', 'brad@gmail.com', '4321', 2, 3, NULL, NULL);

INSERT INTO
    organization (name)
  VALUES
    ('CincySCCA');

INSERT INTO
    season (name, organization_id)
  VALUES
    ('2021', 1);

INSERT INTO
    event (name, date, organization_id, season_id)
  VALUES
    ('T&T #1', '2021-04-10', 1, 1);
