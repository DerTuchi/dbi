import pandas as pd
import duckdb as db
import psycopg2
import os

# Path to the csv files
path_cv = "C:/Users/lenna/Desktop/csv/"
# Path to the sql files. Here: 6a, 9a, 12c, 21a, 25a
path_sql = '/Exercise 1/queries/'

# Read csv files
company_name = pd.read_csv(path_cv + 'company_name.csv',
                           names=('id', 'name', 'country_code', 'imdb_id', 'name_pcode_nf', 'name_pcode_sf', 'md5sum'))
company_type = pd.read_csv(path_cv + 'company_type.csv', names=('id', 'kind'))
info_type = pd.read_csv(path_cv + 'info_type.csv', names=('id', 'info'))
movie_info = pd.read_csv(path_cv + 'movie_info.csv', names=('id', 'movie_id', 'info_type_id', 'info', 'note'))
movie_info_idx = pd.read_csv(path_cv + 'movie_info_idx.csv', names=('id', 'movie_id', 'info_type_id', 'info', 'note'))
title = pd.read_csv(path_cv + 'title.csv', low_memory=False, names=(
    'id', 'title', 'imdb_index', 'kind_ind', 'production_year', 'imdb_id', 'phonetic_code', 'episode_of_id',
    'season_nr', 'episode_nr', 'series_years', 'md5sum'))
movie_companies = pd.read_csv(path_cv + 'movie_companies.csv',
                              names=('id', 'movie_id', 'company_id', 'company_type_id', 'note'))
keyword = pd.read_csv(path_cv + 'keyword.csv', names=('id', 'keyword', 'phonetic_code'))
link_type = pd.read_csv(path_cv + 'link_type.csv', names=('id', 'link'))
movie_keyword = pd.read_csv(path_cv + 'movie_keyword.csv', names=('id', 'movie_id', 'keyword_id'))
movie_link = pd.read_csv(path_cv + 'movie_link.csv', names=('id', 'movie_id', 'linked_movie_id', 'link_type_id'))
cast_info = pd.read_csv(path_cv + 'cast_info.csv', low_memory=False,
                        names=('id', 'person_id', 'movie_id', 'person_role_id', 'note', 'nr_order', 'role_id'))
name = pd.read_csv(path_cv + 'name.csv', low_memory=False, names=(
    'id', 'name', 'imdb_index', 'imdb_id', 'gender', 'name_pcode_cf', 'name_pcode_nf', 'surname_pcode', 'md5sum'))
aka_name = pd.read_csv(path_cv + 'aka_name.csv', low_memory=False, names=(
    'id', 'person_id', 'name', 'imdb_index', 'name_pcode_cf', 'name_pcode_nf', 'surname_pcode', 'md5sum'))
char_name = pd.read_csv(path_cv + 'char_name.csv', low_memory=False,
                        names=('id', 'name', 'imdb_index', 'imdb_id', 'name_pcode_nf', 'surname_pcode', 'md5sum'))
role_type = pd.read_csv(path_cv + 'role_type.csv', names=('id', 'role'))


# Takes a sql query and is able to search the files through the duckdb Package
# and formats the result in a comparable Format
def read_csv(query):
    result = str(db.query(query)).replace("\t", "|").splitlines()[-3].split('|')
    if result.__contains__(''):
        result.remove('')
    return [tuple(result)]


if __name__ == '__main__':
    # Connect to psql db
    conn = psycopg2.connect(host="localhost", database="dbi", user="postgres", password="123")
    cur = conn.cursor()

    for file in os.listdir(path_sql):
        # Open sql file and read content of it
        fd = open(path_sql + file, 'r')
        query = fd.read()
        fd.close()

        try:
            # Apply query from sql file on csv files with 'read_csv' function
            csv_result = read_csv(query)
            # Apply query from sql file on psql db with the cursor
            cur.execute(query)
            psql_result = cur.fetchall()
            print(f'Compare {file} query result with csv-files and psql-db. Check if they are equal. '
                  f'Result: {csv_result.__eq__(psql_result)}')
        except ValueError:
            print(f'{file} could not execute')
