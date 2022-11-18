import pandas as pd
import duckdb as db
import psycopg2
import os

path_cv = "/home/dertuchi/DBI/Init/csv/"
'''
company_name = pd.read_csv(path_cv + 'company_name.csv',
                 names=('id', 'name', 'country_code', 'imdb_id', 'name_pcode_nf', 'name_pcode_sf', 'md5sum'))
company_type = pd.read_csv(path_cv + 'company_type.csv', names=('id', 'kind'))
info_type = pd.read_csv(path_cv + 'info_type.csv', names=('id', 'info'))
movie_info = pd.read_csv(path_cv + 'movie_info.csv', names=('id', 'movie_id', 'info_type_id', 'info', 'note'))
movie_info_idx = pd.read_csv(path_cv + 'movie_info_idx.csv', names=('id', 'movie_id', 'info_type_id', 'info', 'note'))
title = pd.read_csv(path_cv + 'title.csv', low_memory=False, names=(
    'id', 'title', 'imdb_index', 'kind_ind', 'production_year', 'imdb_id', 'phonetic_code', 'episode_of_id',
    'season_nr',
    'episode_nr', 'series_years', 'md5sum'))
movie_companies = pd.read_csv(path_cv + 'movie_companies.csv', names=('id', 'movie_id', 'company_id', 'company_type_id', 'note'))
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
'''


def my_implementation(query):
    result = str(db.query(query)).replace("\t", "|").splitlines()[-3].split('|')
    if result.__contains__(''):
        result.remove('')
    return result



query = "SELECT MIN(k.keyword) AS movie_keyword, " \
            "MIN(n.name) AS actor_name, " \
            "MIN(t.title) As marvel_movie " \
            "FROM ci, k, mk, n, t " \
            "WHERE k.keyword = 'marvel-cinematic-universe' " \
            "AND n.name LIKE '%Downey%Robert%' " \
            "AND t.production_year > 2000 " \
            "AND k.id = mk.keyword_id " \
            "AND t.id = mk.movie_id " \
            "AND t.id = ci.movie_id " \
            "AND ci.movie_id = mk.movie_id " \
            "AND n.id = ci.person_id;"



query_9a = "SELECT MIN(an.name) AS alternative_name, MIN(chn.name) AS character_name, MIN(t.title) AS movie " \
            "FROM an, chn, ci, cn, mc, n, rt, t " \
            "WHERE ci.note IN ('(voice)','(voice: Japanese version)','(voice) (uncredited)','(voice: English version)') " \
            "AND cn.country_code ='[us]' " \
            "AND mc.note IS NOT NULL " \
            "AND (mc.note LIKE '%(USA)%' OR mc.note LIKE '%(worldwide)%') " \
            "AND n.gender ='f' " \
            "AND n.name LIKE '%Ang%' " \
            "AND rt.role ='actress' " \
            "AND t.production_year BETWEEN 2005 AND 2015 " \
            "AND ci.movie_id = t.id " \
            "AND t.id = mc.movie_id " \
            "AND ci.movie_id = mc.movie_id " \
            "AND mc.company_id = cn.id " \
            "AND ci.role_id = rt.id " \
            "AND n.id = ci.person_id " \
            "AND chn.id = ci.person_role_id " \
            "AND an.person_id = n.id " \
            "AND an.person_id = ci.person_id; "

query_25a = "SELECT MIN(mi.info) AS movie_budget, " \
            "MIN(mi_idx.info) AS movie_votes, " \
            "MIN(n.name) AS male_writer, " \
            "MIN(t.title) AS violent_movie_title " \
            "FROM ci, it1, it2, k, mi, mi_idx, mk, n, t " \
            "WHERE ci.note IN ('(writer)','(head writer)','(written by)','(story)','(story editor)')" \
            "AND it1.info = 'genres'" \
            "AND it2.info = 'votes'" \
            "AND k.keyword IN ('murder','blood','gore','death','female-nudity')" \
            "AND mi.info = 'Horror'" \
            "AND n.gender = 'm'" \
            "AND t.id = mi.movie_id " \
            "AND t.id = mi_idx.movie_id " \
            "AND t.id = ci.movie_id " \
            "AND t.id = mk.movie_id " \
            "AND ci.movie_id = mi.movie_id " \
            "AND ci.movie_id = mi_idx.movie_id " \
            "AND ci.movie_id = mk.movie_id " \
            "AND mi.movie_id = mi_idx.movie_id " \
            "AND mi.movie_id = mk.movie_id " \
            "AND mi_idx.movie_id = mk.movie_id " \
            "AND n.id = ci.person_id " \
            "AND it1.id = mi.info_type_id " \
            "AND it2.id = mi_idx.info_type_id " \
            "AND k.id = mk.keyword_id;"

query_21a = "SELECT MIN(cn.name) AS company_name, " \
            "MIN(lt.link) AS link_type, " \
            "MIN(t.title) AS wetsern_follow_up " \
            "FROM cn, ct, k, lt, mc, mi, mk, ml, t " \
            "WHERE cn.country_code !='[pl]' " \
            "AND (cn.name LIKE '%Film%' OR cn.name LIKE '%Warner%') " \
            "AND ct.kind ='production companies' " \
            "AND k.keyword ='sequel' " \
            "AND lt.link LIKE '%follow%' " \
            "AND mc.note IS NULL " \
            "AND mi.info IN ('Sweden', 'Norway', 'Germany', 'Denmark', 'Swedish', 'Denish', 'Norwegian', 'German') " \
            "AND t.production_year BETWEEN 1950 AND 2000 " \
            "AND lt.id = ml.link_type_id " \
            "AND ml.movie_id = t.id " \
            "AND t.id = mk.movie_id " \
            "AND mk.keyword_id = k.id " \
            "AND t.id = mc.movie_id " \
            "AND mc.company_type_id = ct.id " \
            "AND mc.company_id = cn.id " \
            "AND mi.movie_id = t.id " \
            "AND ml.movie_id = mk.movie_id " \
            "AND ml.movie_id = mc.movie_id " \
            "AND mk.movie_id = mc.movie_id " \
            "AND ml.movie_id = mi.movie_id " \
            "AND mk.movie_id = mi.movie_id " \
            "AND mc.movie_id = mi.movie_id;"


query_12c = "SELECT MIN(cn.name) AS movie_company, " \
            "MIN(mi_idx.info) AS rating, " \
            "MIN(t.title) AS mainstream_movie " \
            "FROM company_name AS cn," \
            "company_type AS ct, " \
            "info_type AS it1, " \
            "info_type AS it2, " \
            "movie_companies AS mc, " \
            "movie_info AS mi, " \
            "movie_info_idx AS mi_idx," \
            "title AS t " \
            "WHERE cn.country_code = '[us]' " \
            "AND ct.kind = 'production companies' " \
            "AND it1.info = 'genres' " \
            "AND it2.info = 'rating' " \
            "AND mi.info IN ('Drama', 'Horror', 'Western', 'Family') " \
            "AND mi_idx.info > '7.0' " \
            "AND t.production_year BETWEEN 2000 AND 2010 " \
            "AND t.id = mi.movie_id " \
            "AND t.id = mi_idx.movie_id " \
            "AND mi.info_type_id = it1.id " \
            "AND mi_idx.info_type_id = it2.id " \
            "AND t.id = mc.movie_id " \
            "AND ct.id = mc.company_type_id " \
            "AND cn.id = mc.company_id " \
            "AND mc.movie_id = mi.movie_id " \
            "AND mc.movie_id = mi_idx.movie_id " \
            "AND mi.movie_id = mi_idx.movie_id;"

if __name__ == '__main__':
    dir = '/home/dertuchi/PycharmProjects/dbi/queries/'
    conn = psycopg2.connect(host="localhost", database="dbi", user="postgres", password="123")
    cur = conn.cursor()

    for file in os.listdir(dir):
        fd = open(dir + file, 'r')
        query = fd.read()
        fd.close()

        try:
            my_result = my_implementation(query)
            cur.execute(query)
            psql_result = cur.fetchall()
            print(f'my_result:\t{my_result}\t---\tpsql_result:{psql_result}')
        except ValueError:
            print(f'{file} could not execute')
