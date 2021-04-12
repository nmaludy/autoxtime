#!/bin/bash
cat db/db_table_drop.sql db/db_user_create.sql db/db_table_create.sql db/db_fake_data.sql | sudo -s -u postgres psql autoxtime -f -
