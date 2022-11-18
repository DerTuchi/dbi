# SQL 6a  -> Works
import pandas as pd
import duckdb as db

path_cv = "C:/Users/lenna/Desktop/csv/"

ci = pd.read_csv(path_cv+'cast_info.csv',low_memory=False,names=('id', 'person_id', 'movie_id', 'person_role_id', 'note', 'nr_order', 'role_id'))
k = pd.read_csv(path_cv+'keyword.csv', names=('id', 'keyword', 'phonetic_code'))
mk = pd.read_csv(path_cv+'movie_keyword.csv', names=('id', 'movie_id', 'keyword_id'))
n = pd.read_csv(path_cv+'name.csv',low_memory=False,names=('id','name','imdb_index','imdb_id','gender','name_pcode_cf','name_pcode_nf','surname_pcode','md5sum'))
t = pd.read_csv(path_cv+'title.csv', low_memory=False,names=('id','title','imdb_index','kind_ind','production_year','imdb_id','phonetic_code','episode_of_id','season_nr','episode_nr','series_years','md5sum'))

query = "SELECT MIN(k.keyword) AS movie_keyword, "\
        "MIN(n.name) AS actor_name, "\
        "MIN(t.title) As marvel_movie "\
        "FROM ci, k, mk, n, t "\
        "WHERE k.keyword = 'marvel-cinematic-universe' "\
        "AND n.name LIKE '%Downey%Robert%' "\
        "AND t.production_year > 2000 "\
        "AND k.id = mk.keyword_id "\
        "AND t.id = mk.movie_id "\
        "AND t.id = ci.movie_id "\
        "AND ci.movie_id = mk.movie_id "\
        "AND n.id = ci.person_id;"

result = db.query(query)
print(result)
