# SQL 9a
import pandas as pd
import duckdb as db

path_cv = "C:/Users/lenna/Desktop/csv/"

an = pd.read_csv(path_cv+'aka_name.csv', low_memory=False, names=('id', 'person_id', 'name', 'imdb_index', 'name_pcode_cf', 'name_pcode_nf', 'surname_pcode', 'md5sum'))
chn = pd.read_csv(path_cv+'char_name.csv', low_memory=False, names=('id', 'name', 'imdb_index', 'imdb_id', 'name_pcode_nf', 'surname_pcode', 'md5sum'))
ci = pd.read_csv(path_cv+'cast_info.csv', low_memory=False, names=('id', 'person_id', 'movie_id', 'person_role_id', 'note', 'nr_order', 'role_id'))
cn = pd.read_csv(path_cv+'company_name.csv', names=('id', 'cn_name', 'country_code', 'imdb_id', 'name_pcode_nf', 'name_pcode_sf', 'md5sum'))
mc = pd.read_csv(path_cv+'movie_companies.csv', names=('id', 'movie_id', 'company_id', 'company_type_id', 'note'))
n = pd.read_csv(path_cv+'name.csv', low_memory=False, names=('id','name','imdb_index','imdb_id','gender','name_pcode_cf', 'name_pcode_nf', 'surname_pcode', 'md5sum'))
rt = pd.read_csv(path_cv+'role_type.csv', names=('id', 'role'))
t = pd.read_csv(path_cv+'title.csv', low_memory=False, names=('id','title','imdb_index','kind_ind','production_year', 'imdb_id','phonetic_code','episode_of_id','season_nr','episode_nr','series_years','md5sum'))


query = "SELECT MIN(an.name) AS alternative_name, MIN(chn.name) AS character_name, MIN(t.title) AS movie "\
        "FROM an, chn, ci, cn, mc, n, rt, t "\
        "WHERE ci.note IN ('(voice)','(voice: Japanese version)','(voice) (uncredited)','(voice: English version)') "\
        "AND cn.country_code ='[us]' "\
        "AND mc.note IS NOT NULL "\
        "AND (mc.note LIKE '%(USA)%' OR mc.note LIKE '%(worldwide)%') "\
        "AND n.gender ='f' "\
        "AND n.name LIKE '%Ang%' "\
        "AND rt.role ='actress' "\
        "AND t.production_year BETWEEN 2005 AND 2015 "\
        "AND ci.movie_id = t.id "\
        "AND t.id = mc.movie_id "\
        "AND ci.movie_id = mc.movie_id "\
        "AND mc.company_id = cn.id "\
        "AND ci.role_id = rt.id "\
        "AND n.id = ci.person_id "\
        "AND chn.id = ci.person_role_id "\
        "AND an.person_id = n.id "\
        "AND an.person_id = ci.person_id; "


result = db.query(query)
print(result)
