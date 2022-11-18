# SQL 21a
import pandas as pd
import duckdb as db

path_cv = "C:/Users/lenna/Desktop/csv/"

cn = pd.read_csv(path_cv+'company_name.csv', names=('id', 'name', 'country_code', 'imdb_id', 'name_pcode_nf', 'name_pcode_sf', 'md5sum'))
ct = pd.read_csv(path_cv+'company_type.csv', names=('id', 'kind'))
k = pd.read_csv(path_cv+'keyword.csv', names=('id', 'keyword', 'phonetic_code'))
lt = pd.read_csv(path_cv+'link_type.csv', names=('id', 'link'))
mc = pd.read_csv(path_cv+'movie_companies.csv', names=('id', 'movie_id', 'company_id', 'company_type_id', 'note'))
mi = pd.read_csv(path_cv+'movie_info.csv', names=('id', 'movie_id', 'info_type_id', 'info', 'note'))
mk = pd.read_csv(path_cv+'movie_keyword.csv', names=('id', 'movie_id', 'keyword_id'))
ml = pd.read_csv(path_cv+'movie_link.csv', names=('id', 'movie_id', 'linked_movie_id', 'link_type_id'))
t = pd.read_csv(path_cv+'title.csv', low_memory=False,names=('id','title','imdb_index','kind_ind','production_year','imdb_id','phonetic_code','episode_of_id','season_nr','episode_nr','series_years','md5sum'))

query = "SELECT MIN(cn.name) AS company_name, "\
        "MIN(lt.link) AS link_type, "\
        "MIN(t.title) AS wetsern_follow_up "\
        "FROM cn, ct, k, lt, mc, mi, mk, ml, t "\
        "WHERE cn.country_code !='[pl]' "\
        "AND (cn.name LIKE '%Film%' OR cn.name LIKE '%Warner%') "\
        "AND ct.kind ='production companies' "\
        "AND k.keyword ='sequel' "\
        "AND lt.link LIKE '%follow%' "\
        "AND mc.note IS NULL "\
        "AND mi.info IN ('Sweden', 'Norway', 'Germany', 'Denmark', 'Swedish', 'Denish', 'Norwegian', 'German') "\
        "AND t.production_year BETWEEN 1950 AND 2000 "\
        "AND lt.id = ml.link_type_id "\
        "AND ml.movie_id = t.id "\
        "AND t.id = mk.movie_id "\
        "AND mk.keyword_id = k.id "\
        "AND t.id = mc.movie_id "\
        "AND mc.company_type_id = ct.id "\
        "AND mc.company_id = cn.id "\
        "AND mi.movie_id = t.id "\
        "AND ml.movie_id = mk.movie_id "\
        "AND ml.movie_id = mc.movie_id "\
        "AND mk.movie_id = mc.movie_id "\
        "AND ml.movie_id = mi.movie_id "\
        "AND mk.movie_id = mi.movie_id "\
        "AND mc.movie_id = mi.movie_id;"

result = db.query(query)
print(result)
