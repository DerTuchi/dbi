#SQL 10a
import pandas as pd
import duckdb as db
path_cv = "C:/Users/lenna/Desktop/csv/"

cn = pd.read_csv(path_cv+'company_name.csv', names=('id', 'name', 'country_code', 'imdb_id', 'name_pcode_nf','name_pcode_sf', 'md5sum'))
ct = pd.read_csv(path_cv+'company_type.csv', names=('id', 'kind'))
it1 = pd.read_csv(path_cv+'info_type.csv', names=('id', 'info'))
it2 = pd.read_csv(path_cv+'info_type.csv', names=('id', 'info'))
mi = pd.read_csv(path_cv+'movie_info.csv', names=('id','movie_id','info_type_id','info','note'))
mi_idx = pd.read_csv(path_cv+'movie_info_idx.csv', names=('id', 'movie_id','info_type_id','info','note'))
t = pd.read_csv(path_cv+'title.csv', low_memory=False,names=('id','title','imdb_index','kind_ind','production_year','imdb_id','phonetic_code','episode_of_id','season_nr','episode_nr','series_years','md5sum'))
mc = pd.read_csv(path_cv+'movie_companies.csv', names=('id','movie_id','company_id','company_type_id','note'))

query = "SELECT MIN(cn.name) AS movie_company, "\
        "MIN(mi_idx.info) AS rating, "\
        "MIN(t.title) AS mainstream_movie "\
        "FROM cn,ct, it1, it2, mc, mi, mi_idx, t "\
        "WHERE cn.country_code = '[us]' "\
        "AND ct.kind = 'production companies' "\
        "AND it1.info = 'genres' "\
        "AND it2.info = 'rating' "\
        "AND mi.info IN ('Drama', 'Horror', 'Western', 'Family') "\
        "AND mi_idx.info > '7.0' "\
        "AND t.production_year BETWEEN 2000 AND 2010 "\
        "AND t.id = mi.movie_id "\
        "AND t.id = mi_idx.movie_id "\
        "AND mi.info_type_id = it1.id "\
        "AND mi_idx.info_type_id = it2.id "\
        "AND t.id = mc.movie_id "\
        "AND ct.id = mc.company_type_id "\
        "AND cn.id = mc.company_id "\
        "AND mc.movie_id = mi.movie_id "\
        "AND mc.movie_id = mi_idx.movie_id "\
        "AND mi.movie_id = mi_idx.movie_id;"

result = db.query(query)
print(result)
