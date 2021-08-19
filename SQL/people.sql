--
-- Asymptomatic COVID monitoring
--

quit;  -- This is not valid syntax but it will stop me from 
       -- accidentally executing this whole document as a script, which would be bad.

create table People (
   pid                text,
   isStudent          boolean,
   firstName          text, 
   lastName           text, 
   major              text,
   email1             text,
   email2             text,
   phone1             text,
   phone2             text,
   livesOnCampus      boolean,
   dormBuilding       text,
   dormFloor          int,
   dormRoom           text,
   dormBuildingDesc   text,
   isEmployee         boolean,
   empDept            text,
   empTitle           text, 
   empPhone           text,
   FTE                text, 
   isRemoteEmp        boolean, 
   zipCode            text,
 primary key (pid)
);

-- Look at the data from the import table to People
select * 
from People
where pid in (select cwid from importTable);


-- Do some counts. Make a Venn diagram to check the numbers.

-- Students and non-students
select isStudent, count(pid) 
from People
group by isStudent
order by isStudent ASC;

-- Employees and non-employees
select isEmployee, count(pid) 
from People
group by isEmployee
order by isEmployee ASC;

-- People who are both a student AND an employee
select count(pid)
from People 
where isStudent 
  and isEmployee;

-- Students who ARE NOT employees
select count(pid)
from People 
where isStudent and (not isEmployee);

-- Employees who ARE NOT students
select count(pid)
from People 
where (not isStudent) and isEmployee;

-- People who are both a student AND an employee BY fte
select FTE, count(pid)
from People 
where isStudent 
  and isEmployee
group by FTE;

-- Employees who ARE NOT students who DO NOT live on campus
select count(pid)
from People 
where (not isStudent) 
  and isEmployee
  and (not livesOnCampus);

-- Counts for those who don't and do live on campus.
select livesOnCampus, count(pid) 
from People
group by livesOnCampus
order by livesOnCampus ASC;

-- Students who live on campus
select count(pid) 
from People
where isStudent
  and livesOnCampus;

-- Students who DO NOT live on campus (remote / commuters)
select count(pid) 
from People
where isStudent
  and not livesOnCampus;

-- Look at the Dorm data.
select distinct(dormBuilding)
from People
order by dormBuilding ASC;

-- Look at the Dorm and DESC data.
select distinct(dormBuilding), dormBuildingDesc
from People;

-- Check some logic.
select *
from people 
where dormBuilding is null
  and livesOnCampus;
-- Should be none.

-- Commuter students
select *
from people 
where isStudent
  and dormBuilding is null;
-- Compare to value above. If they differ, 
-- look for those people.
select *
from people p
where isStudent
  and dormBuilding is not null
  and p.pid in ( select pid
                 from People
                 where isStudent
                   and not livesOnCampus );
-- Here are those other people.
select *
from people 
where isStudent
  and not livesOnCampus
  and dormBuilding is not null;

-- Remote and/or Commuter students. Should be same as above.
select *
from people 
where isStudent
  and not livesOnCampus;

-- Student employees who live on campus.  
select *
from People 
where isStudent 
  and isEmployee
  and livesOnCampus;


-- 
-- Overview and sanity checking
-- 

-- Total People on campus and off campus
select livesOnCampus, count (pid)
from People
group by livesOnCampus;

-- People on campus subtotaled by dorm. 
-- Use this to make a Dorms view.
select dormBuilding, count (pid) as subtotal
from People
where livesOnCampus 
group by dormBuilding
order by dormBuilding ASC;

create or replace view Dorms 
as 
select dormBuilding, count (pid) as "occupied"
from People
where livesOnCampus 
group by dormBuilding
order by dormBuilding ASC;

-- Check it.
select *
from Dorms;


-- People on campus subtotaled by dorm and floor
select dormBuilding, dormFloor, count (pid) as subtotal
from People
where livesOnCampus 
group by dormBuilding, dormFloor
order by dormBuilding, dormFloor;

-- Count of people living on campus
select sum(subtotal)
from (select dormBuilding, dormFloor, count (pid) as subtotal
      from People
      where livesOnCampus
      group by dormBuilding, dormFloor
      order by dormBuilding, dormFloor) sub1;

-- Count of STUDENTS
select count(pid)
from People
where isStudent;

-- Count of STUDENTS on campus
select count(pid)
from People
where livesOnCampus
  and isStudent;

-- Count of STUDENTS off campus
select count(pid)
from People
where not livesOnCampus
  and isStudent;

-- Count of EMPLOYEES off campus
select count(pid)
from People
where not livesOnCampus
  and not isStudent
  and isEmployee;

-- Count of Full-time EMPLOYEES off campus
select count(pid)
from People
where not livesOnCampus
  and not isStudent
  and isEmployee
  and FTE = 'F';

-- Count of Part-time EMPLOYEES off campus
select count(pid)
from People
where not livesOnCampus
  and not isStudent
  and isEmployee
  and FTE = 'P';

-- Count of ALL people off campus
select count(pid)
from People
where not livesOnCampus;

-- Count of student and employee
select count(pid)
from People
where isStudent
  and isEmployee;

-- Count of student and NOT employee
select count(pid)
from People
where isStudent
  and not isEmployee;

-- Count of NOT student and employee
select count(pid)
from People
where not isStudent
  and isEmployee;
