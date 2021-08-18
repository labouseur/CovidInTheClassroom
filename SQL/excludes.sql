--
-- People excluded from surveillance testing
--

-- Table for those excluded from monitoring.
create table Excludes (
   pid          text not null,
   excludeStart date not null,
   excludeStop  date,
	notes        text,
   updated      date not null,
 primary key (pid)
);

-- View for those CURRENTLY excluded from monitoring.
create or replace view CurrentlyExcluded
as
select pid
from Excludes
where excludeStop is null
  or now()::date between excludeStart and excludeStop;  -- These are DATE and not TIMESTAMPs, so this between should be fine.

-- Check the table.
select *
from Excludes;

-- Check the view.
select *
from CurrentlyExcluded;

-- Count of ALL CWIDs CURRENTLY excluded, whether or not they are in the People table.
select count(pid)
from CurrentlyExcluded;

-- People CURRENTLY excluded that ARE ALSO in the People table
select count(*)
from CurrentlyExcluded 
where pid in (select pid from people);

-- People CURRENTLY excluded that ARE NOT in the People table
select count(*)
from CurrentlyExcluded 
where pid not in (select pid from people);
-- Who are they?
select notes, count(*)
from Excludes 
where pid in (select pid from CurrentlyExcluded)
  and pid not in (select pid from people)
group by notes
order by count(*) DESC;

-- Currently excluded People by location/dormBuilding
select count(*), dormBuilding
from CurrentlyExcluded e inner join People p on e.pid = p.pid
group by dormBuilding
order by dormBuilding ASC;

-- All CURRENTLY excluded people in the People table by re-inclusion date
select pid, excludeStop
from Excludes
where excludeStop is not null
  and pid in (select pid from CurrentlyExcluded)
  and pid in (select pid from People)
order by excludeStop ASC;

-- ALL (current and past, perhaps not currently excluded) exclusion counts by month of re-inclusion
-- for people in the People table
select count(pid), 
       extract(month from excludeStop)::text || ' - ' || 
		 coalesce( to_char( to_date((extract(month from excludeStop)::text), 'MM'), 'Month'), 'No re-inclusion date') as "ReIncludeMonth"
from Excludes
where pid in (select pid from People)
group by "ReIncludeMonth"
order by "ReIncludeMonth" ASC;
