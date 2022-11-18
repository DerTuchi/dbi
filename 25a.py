# SQL 25a
import pandas as pd
import duckdb as db

path_cv = "C:/Users/lenna/Desktop/csv/"

ci = pd.read_csv(path_cv+'cast_info.csv',low_memory=False,names=('id', 'person_id', 'movie_id', 'person_role_id', 'note', 'nr_order', 'role_id'))
it = pd.read_csv(path_cv+'info_type.csv', names=('id', 'info'))
k = pd.read_csv(path_cv+'keyword.csv', names=('id', 'keyword', 'phonetic_code'))
mi = pd.read_csv(path_cv+'movie_info.csv', names=('id', 'movie_id', 'info_type_id', 'info', 'note'))
mi_idx = pd.read_csv(path_cv+'movie_info_idx.csv', names=('id', 'movie_id','info_type_id','info','note'))
mk = pd.read_csv(path_cv+'movie_keyword.csv', names=('id', 'movie_id', 'keyword_id'))
n = pd.read_csv(path_cv+'name.csv',low_memory=False,names=('id','name','imdb_index','imdb_id','gender','name_pcode_cf','name_pcode_nf','surname_pcode','md5sum'))
t = pd.read_csv(path_cv+'title.csv', low_memory=False,names=('id','title','imdb_index','kind_ind','production_year','imdb_id','phonetic_code','episode_of_id','season_nr','episode_nr','series_years','md5sum'))

it1 = it.loc[it['info'] == 'genres']
it2 = it.loc[it['info'] == 'votes']


query = "SELECT MIN(mi.info) AS movie_budget, " \
        "MIN(mi_idx.info) AS movie_votes, " \
        "MIN(n.name) AS male_writer, "\
        "MIN(t.title) AS violent_movie_title "\
        "FROM ci, it1, it2, k, mi, mi_idx, mk, n, t "\
        "WHERE ci.note IN ('(writer)','(head writer)','(written by)','(story)','(story editor)')"\
        "AND it1.info = 'genres'"\
        "AND it2.info = 'votes'"\
        "AND k.keyword IN ('murder','blood','gore','death','female-nudity')"\
        "AND mi.info = 'Horror'"\
        "AND n.gender = 'm'"\
        "AND t.id = mi.movie_id "\
        "AND t.id = mi_idx.movie_id "\
        "AND t.id = ci.movie_id "\
        "AND t.id = mk.movie_id "\
        "AND ci.movie_id = mi.movie_id "\
        "AND ci.movie_id = mi_idx.movie_id "\
        "AND ci.movie_id = mk.movie_id "\
        "AND mi.movie_id = mi_idx.movie_id "\
        "AND mi.movie_id = mk.movie_id "\
        "AND mi_idx.movie_id = mk.movie_id "\
        "AND n.id = ci.person_id "\
        "AND it1.id = mi.info_type_id "\
        "AND it2.id = mi_idx.info_type_id "\
        "AND k.id = mk.keyword_id;"

moviebudget = db.query(query)
print(moviebudget)
