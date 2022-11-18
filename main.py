import pandas as pd
import duckdb as db

path_cv = "C:/Users/lenna/Desktop/csv/"

cn = pd.read_csv(path_cv + 'company_name.csv',
                 names=('id', 'name', 'country_code', 'imdb_id', 'name_pcode_nf', 'name_pcode_sf', 'md5sum'))
ct = pd.read_csv(path_cv + 'company_type.csv', names=('id', 'kind'))
it = pd.read_csv(path_cv + 'info_type.csv', names=('id', 'info'))
it1 = pd.read_csv(path_cv + 'info_type.csv', names=('id', 'info'))
it2 = pd.read_csv(path_cv + 'info_type.csv', names=('id', 'info'))
mi = pd.read_csv(path_cv + 'movie_info.csv', names=('id', 'movie_id', 'info_type_id', 'info', 'note'))
mi_idx = pd.read_csv(path_cv + 'movie_info_idx.csv', names=('id', 'movie_id', 'info_type_id', 'info', 'note'))
t = pd.read_csv(path_cv + 'title.csv', low_memory=False, names=(
    'id', 'title', 'imdb_index', 'kind_ind', 'production_year', 'imdb_id', 'phonetic_code', 'episode_of_id',
    'season_nr',
    'episode_nr', 'series_years', 'md5sum'))
mc = pd.read_csv(path_cv + 'movie_companies.csv', names=('id', 'movie_id', 'company_id', 'company_type_id', 'note'))
k = pd.read_csv(path_cv + 'keyword.csv', names=('id', 'keyword', 'phonetic_code'))
lt = pd.read_csv(path_cv + 'link_type.csv', names=('id', 'link'))
mk = pd.read_csv(path_cv + 'movie_keyword.csv', names=('id', 'movie_id', 'keyword_id'))
ml = pd.read_csv(path_cv + 'movie_link.csv', names=('id', 'movie_id', 'linked_movie_id', 'link_type_id'))
ci = pd.read_csv(path_cv + 'cast_info.csv', low_memory=False,
                 names=('id', 'person_id', 'movie_id', 'person_role_id', 'note', 'nr_order', 'role_id'))
n = pd.read_csv(path_cv + 'name.csv', low_memory=False, names=(
    'id', 'name', 'imdb_index', 'imdb_id', 'gender', 'name_pcode_cf', 'name_pcode_nf', 'surname_pcode', 'md5sum'))
an = pd.read_csv(path_cv + 'aka_name.csv', low_memory=False, names=(
    'id', 'person_id', 'name', 'imdb_index', 'name_pcode_cf', 'name_pcode_nf', 'surname_pcode', 'md5sum'))
chn = pd.read_csv(path_cv + 'char_name.csv', low_memory=False,
                  names=('id', 'name', 'imdb_index', 'imdb_id', 'name_pcode_nf', 'surname_pcode', 'md5sum'))
rt = pd.read_csv(path_cv + 'role_type.csv', names=('id', 'role'))


def refactor(query):
    temp = str(db.query(query)).replace("\t", "|").splitlines()
    names = temp[-6].split('|')
    values = temp[-3].split('|')
    try:
        names.remove('')
        values.remove('')
    except:
        pass
    return [names, values]


def benchmark_6a():
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
    return refactor(query)

def benchmark_9a():
    query = "SELECT MIN(an.name) AS alternative_name, MIN(chn.name) AS character_name, MIN(t.title) AS movie " \
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
    return refactor(query)


def benchmark_25a():
    query = "SELECT MIN(mi.info) AS movie_budget, " \
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
    return refactor(query)


def benchmark_21a():
    query = "SELECT MIN(cn.name) AS company_name, " \
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
    return refactor(query)


def benchmark_12c():
    query = "SELECT MIN(cn.name) AS movie_company, " \
            "MIN(mi_idx.info) AS rating, " \
            "MIN(t.title) AS mainstream_movie " \
            "FROM cn,ct, it1, it2, mc, mi, mi_idx, t " \
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
    return refactor(query)


if __name__ == '__main__':
    print(benchmark_6a(), '\n', benchmark_9a(), '\n', benchmark_25a(), '\n', benchmark_21a(), '\n', benchmark_12c())
