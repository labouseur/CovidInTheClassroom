--
-- Spring 2021 testing/monitoring 
--

-- Is everybody assigned to a monitor group?
select *
from People
where monitorGroup not in ('A', 'B')
   or monitorGroup is null;
-- Should be none.

-- Check the monitor group A/B balance.
select monitorGroup, count(pid)
from People
group by monitorGroup
order by monitorGroup ASC;

-- A/B grouping for everybody by "dorm building"
select dormBuilding, monitorGroup, count(pid)
from People
group by dormBuilding, monitorGroup
order by dormBuilding ASC, monitorGroup ASC;

-- A/B balance for everybody by monitorGroup
select monitorGroup, dormBuilding, count(pid)
from People
group by monitorGroup, dormBuilding
order by monitorGroup ASC, dormBuilding ASC;

-- Look at the on-campus student data.
select pid, dormBuilding, dormFloor, dormRoom, monitorGroup 
from People
where isStudent
  and livesOnCampus
order by dormBuilding ASC, dormFloor ASC, dormRoom ASC;

-- A/B on-campus student grouping by dorm and floor
select dormBuilding, dormFloor, monitorGroup, count(pid)
from People
where isStudent
  and livesOnCampus
group by dormBuilding, dormFloor, monitorGroup
order by dormBuilding ASC, dormFloor ASC, monitorGroup ASC;

-- A/B grouping for off-campus people by count in zip code.
select substring(zipcode from 1 for 5), monitorGroup, count(pid)
from People
group by substring(zipcode from 1 for 5), monitorGroup
order by count(pid) DESC, substring(zipcode from 1 for 5) ASC, monitorGroup ASC;

-- A/B grouping for off-campus people by zip code.
select substring(zipcode from 1 for 5), monitorGroup, count(pid)
from People
group by substring(zipcode from 1 for 5), monitorGroup
order by substring(zipcode from 1 for 5) ASC, monitorGroup ASC;

-- A/B balance overall (again)
select monitorGroup, count(pid)
from People
group by monitorGroup;

--
-- Okay, ready to test.
--

-- drop function assignTestTimes(text, bool, date);
create or replace function assignTestTimes(theGroup       text, 
														 resetWorkTable bool, 
														 startDate      date) 
  returns table (
      monitorGroup       text,
	   inviteDate         timestamp,
	   inviteDateText     text,
	   pid                text,
      firstName          text, 
      lastName           text, 
      email1             text,
      email2             text,
      phone1             text,
      phone2             text,
	   isStudent          boolean,
      isEmployee         boolean,
	   livesOnCampus      boolean,
 	   major              text,
      dormBuilding       text,
      dormFloor          int,
      dormRoom           text,
      empDept            text,
      empTitle           text, 
      empPhone           text
	) 
language plpgsql
as
$$
declare
   groupSize         int;
   groupTime         int;   
	groupCount        int;
   startTimeWeekDAY  time without time zone;
   startTimeWeekEND  time without time zone;
	peopleInThisGroup int;
	dailyTestingLoad  int;
	onCampusStudents  int;
	offCampusStudents int;
   employees         int;
   weekEndStudents   int;
   weekDayStudents   int;
	weekDayCommuters  int;
	weekDayEmployees  int;
   _inviteTime       time without time zone;
   _inviteDate       date;
   _inviteDateTime   timestamp without time zone;
   _inviteOffset     interval;
   personRow         record;
	endDate           date;
	currentDate       date;
	dayKind           text;
begin
   -- Reset (create a new) workTable if necessary.
	if resetWorkTable then
		drop table if exists workTable;
		create table workTable  
		as
		select null::text as "monitorGroup",
				 null::timestamp as "inviteDate",
				 null::text as "inviteDateText",
				 p.pid, 
				 p.firstName, 
				 p.lastName, 
				 coalesce(p.email1, '') as "email1", 
				 coalesce(p.email2, '') as "email2",
				 coalesce(p.phone1, '') as "phone1", 
				 coalesce(p.phone2, '') as "phone2",
				 p.isStudent,
				 p.isEmployee,			 
				 p.livesOnCampus,
				 p.major,
				 p.dormBuilding,  
				 p.dormFloor,
				 p.dormRoom,
				 p.empDept,
				 p.empTitle,
				 p.empPhone
		from PeopleToBeTested p
		where false; 
   end if;
	
   -- Set some parameters.
   groupSize   := 80; -- people per time slot.
   groupTime   := 20; -- minuntes per slot.
   startTimeWeekDAY := '4:00pm';  -- 16:00:00
	startTimeWeekEND := '10:00am'; -- 10:00:00
	
	-- Compute the rest.
   currentDate := startDate;
	endDate := startDate + interval '6 days';
	
	peopleInThisGroup := (select count(p.pid) from PeopleToBeTested p where p.monitorGroup = theGroup);
	dailyTestingLoad  := peopleInThisGroup / 5;  -- We'll rely on the integer data type to make this work and cause rounding (or truncation). See below.
	onCampusStudents  := (select count(p.pid) from PeopleToBeTested p where p.monitorGroup = theGroup and p.isStudent and p.livesOnCampus);
	offCampusStudents := (select count(p.pid) from PeopleToBeTested p where p.monitorGroup = theGroup and p.isStudent and not p.livesOnCampus);
   employees         := (select count(p.pid) from PeopleToBeTested p where p.monitorGroup = theGroup and not p.isStudent and not p.livesOnCampus and p.isEmployee);

   raise notice '% non-excluded people in monitor group % means the testing load is % people per week day.', peopleInThisGroup, theGroup, dailyTestingLoad;
	raise notice 'There are % non-excluded on-campus students, % non-excluded off-campus students, and % non-excluded employees in group %.', onCampusStudents, offCampusStudents, employees ,theGroup;
	
	weekEndStudents  := dailyTestingLoad;  -- We only test on-campus students on the weekends.
   weekDayStudents  := 1 + (onCampusStudents - (weekEndStudents * 2)) / 5;
	weekDayCommuters := 1 + (offCampusStudents / 5);
	weekDayEmployees := 1 + (employees / 5);
	-- We add 1 to the above three assignments to account for the rounding/truncation error caused by int division.
	
	raise notice 'Testing % on-campus students every weekEND.', weekEndStudents;
	raise notice 'Testing % on-campus students every weekDAY.', weekDayStudents;
	raise notice 'Testing % off-campus commuters every weekDAY.', weekDayCommuters;
	raise notice 'Testing % employees every weekDAY.', weekDayEmployees;

	raise notice 'Generating test invitation times in groups of size % spaced % minutes apart', groupSize, groupTime;
   raise notice 'starting at % on weekdays and % on weekends.', startTimeWeekDAY, startTimeWeekEND;

	-- Loop over the week from start date to the end date.
   while currentDate <= endDate loop
	   groupCount := 0;
	   
		if (lower(trim(to_char(currentDate, 'Day'))) = 'saturday') or 
		   (lower(trim(to_char(currentDate, 'Day'))) = 'sunday')   then 
		   dayKind := 'weekEND';
			
			-- It's a weekEND so we want <weekEndStudents> non-excluded on-campus students.
			for personRow in ( select *
									 from PeopleToBeTested p 
									 where p.monitorGroup = theGroup
									   and p.isStudent 
									   and p.livesOnCampus 
									   and p.pid not in (select w.pid from worktable w)
									 order by p.dormBuilding ASC, 
									 			 p.dormFloor ASC,  
									 			 p.dormRoom ASC 
									 limit weekEndStudents) loop

				_inviteOffset := ( (groupCount / groupSize) * groupTime) * interval '1' minute; -- "/" is the DIV operator.
				_inviteTime := startTimeWeekEND + _inviteOffset;
            _inviteDate := currentDate;
				_inviteDateTime := to_timestamp ( cast(_inviteDate as text) || ' ' || _inviteTime, 'YYYY-MM-DD HH24:MI:SS');

				insert into worktable
				select theGroup as "monitorGroup",
				       _inviteDateTime as "inviteDate",
						 to_char(_inviteDateTime, 'FMDay, FMMonth FMDDth, YYYY at FMHH12:MI pm') as "inviteDateText",
						 p.pid, 
						 p.firstName, 
						 p.lastName, 
						 coalesce(p.email1, '') as "email1", 
						 coalesce(p.email2, '') as "email2",
						 coalesce(p.phone1, '') as "phone1", 
						 coalesce(p.phone2, '') as "phone2",
						 p.isStudent,
						 p.isEmployee,			 
						 p.livesOnCampus,
						 p.major,
						 p.dormBuilding,  
						 p.dormFloor,
						 p.dormRoom,
						 p.empDept,
						 p.empTitle,
						 p.empPhone
				from PeopleToBeTested p
				where p.pid = personRow.pid; 

				groupCount := groupCount + 1;			
			end loop; -- for personRow 
		
		else -- It's a weekDAY		
		   dayKind := 'weekDAY';

			-- It's a weekDAY so we want <weekDayStudents>  non-excluded on-campus  students,
			--                           <weekDayCommuters> non-excluded off-campus students, and
			--                           <weekDayEmployees> non-excluded employees

			-- FIRST: <weekDayEmployees> employees by zip code
			for personRow in ( select *
									 from PeopleToBeTested p 
									 where p.monitorGroup = theGroup
									   and not p.isStudent 
									   and not p.livesOnCampus 
									   and p.isEmployee
									   and p.pid not in (select w.pid from worktable w)
									 order by p.zipCode ASC
									 limit weekDayEmployees) loop
			
				_inviteOffset := ( (groupCount / groupSize) * groupTime) * interval '1' minute; -- "/" is the DIV operator.
				_inviteTime := startTimeWeekDAY + _inviteOffset;
				_inviteDate := currentDate;
				_inviteDateTime := to_timestamp ( cast(_inviteDate as text) || ' ' || _inviteTime, 'YYYY-MM-DD HH24:MI:SS');

				insert into worktable
				select theGroup as "monitorGroup",
				       _inviteDateTime as "inviteDate",
						 to_char(_inviteDateTime, 'FMDay, FMMonth FMDDth, YYYY at FMHH12:MI pm') as "inviteDateText",
						 p.pid, 
						 p.firstName, 
						 p.lastName, 
						 coalesce(p.email1, '') as "email1", 
						 coalesce(p.email2, '') as "email2",
						 coalesce(p.phone1, '') as "phone1", 
						 coalesce(p.phone2, '') as "phone2",
						 p.isStudent,
						 p.isEmployee,			 
						 p.livesOnCampus,
						 p.major,
						 p.dormBuilding,  
						 p.dormFloor,
						 p.dormRoom,
						 p.empDept,
						 p.empTitle,
						 p.empPhone
				from PeopleToBeTested p
				where p.pid = personRow.pid; 

				groupCount := groupCount + 1;			
			end loop; -- for personRow 

			-- SECOND: <weekDayCommuters> off-campus students by zip code
			for personRow in ( select *
									 from PeopleToBeTested p 
									 where p.monitorGroup = theGroup
									   and p.isStudent 
									   and not p.livesOnCampus 
									   and p.pid not in (select w.pid from worktable w)
									 order by p.zipCode ASC
									 limit weekDayCommuters) loop
			
				_inviteOffset := ( (groupCount / groupSize) * groupTime) * interval '1' minute; -- "/" is the DIV operator.
				_inviteTime := startTimeWeekDAY + _inviteOffset;
				_inviteDate := currentDate;
				_inviteDateTime := to_timestamp ( cast(_inviteDate as text) || ' ' || _inviteTime, 'YYYY-MM-DD HH24:MI:SS');

				insert into worktable
				select theGroup as "monitorGroup",
				       _inviteDateTime as "inviteDate",
						 to_char(_inviteDateTime, 'FMDay, FMMonth FMDDth, YYYY at FMHH12:MI pm') as "inviteDateText",
						 p.pid, 
						 p.firstName, 
						 p.lastName, 
						 coalesce(p.email1, '') as "email1", 
						 coalesce(p.email2, '') as "email2",
						 coalesce(p.phone1, '') as "phone1", 
						 coalesce(p.phone2, '') as "phone2",
						 p.isStudent,
						 p.isEmployee,			 
						 p.livesOnCampus,
						 p.major,
						 p.dormBuilding,  
						 p.dormFloor,
						 p.dormRoom,
						 p.empDept,
						 p.empTitle,
						 p.empPhone
				from PeopleToBeTested p
				where p.pid = personRow.pid; 

				groupCount := groupCount + 1;			
			end loop; -- for personRow 

			-- THIRD: <weekDayStudents> on-campus students by dorm room
			for personRow in ( select *
									 from PeopleToBeTested p 
									 where p.monitorGroup = theGroup
									   and p.isStudent 
									   and p.livesOnCampus 
									   and p.pid not in (select w.pid from worktable w)
									 order by p.dormBuilding ASC, 
									 			 p.dormFloor ASC,  
									 			 p.dormRoom ASC 
									 limit weekDayStudents) loop
			
				_inviteOffset := ( (groupCount / groupSize) * groupTime) * interval '1' minute; -- "/" is the DIV operator.
				_inviteTime := startTimeWeekDAY + _inviteOffset;
				_inviteDate := currentDate;
				_inviteDateTime := to_timestamp ( cast(_inviteDate as text) || ' ' || _inviteTime, 'YYYY-MM-DD HH24:MI:SS');

				insert into worktable
				select theGroup as "monitorGroup",
				       _inviteDateTime as "inviteDate",
						 to_char(_inviteDateTime, 'FMDay, FMMonth FMDDth, YYYY at FMHH12:MI pm') as "inviteDateText",
						 p.pid, 
						 p.firstName, 
						 p.lastName, 
						 coalesce(p.email1, '') as "email1", 
						 coalesce(p.email2, '') as "email2",
						 coalesce(p.phone1, '') as "phone1", 
						 coalesce(p.phone2, '') as "phone2",
						 p.isStudent,
						 p.isEmployee,			 
						 p.livesOnCampus,
						 p.major,
						 p.dormBuilding,  
						 p.dormFloor,
						 p.dormRoom,
						 p.empDept,
						 p.empTitle,
						 p.empPhone
				from PeopleToBeTested p
				where p.pid = personRow.pid; 

				groupCount := groupCount + 1;			
			end loop; -- for personRow 

		end if; -- It's a weekEND or a weekDAY

		raise notice 'Processed % %, a %. Group count = %', to_char(currentDate, 'Day'), currentDate, dayKind, groupCount;
		
		currentDate := currentDate + interval '1 day';				
	end loop; -- while currentDate <= endDate
	
	-- That's it; we're done. Return the table.
   return query (select * from workTable); 
end;
$$

-- The parameter is the calendar date of the MONDAY of the testing week. 
-- The logical parameter is whether or not to reset the worktable.
-- Be VERY CAREFUL about these.
select * from assignTestTimes('A', true,  '2021-05-03');  
select * from assignTestTimes('B', false, '2021-05-10');  

select count(*) from workTable;
-- Check this against the total number of People minus 
-- the count of people currently excluded who are also in the People table and those in quarantine or isolation.

-- Look for people NOT in the workTable.
select * -- count(*)
from PeopleToBeTested
where monitorGroup in ('A', 'B')
  and pid not in (select pid from worktable);
-- Should be 0, assuming we generated schedules for groups A and B.

-- Check the uniqueness of all PIDs in the worktable, just to be sure.
select count(distinct(pid)) from worktable;

-- Did we get all of the non-excluded on-campus students in the work table?
select count(pid)
from PeopleToBeTested
where isStudent
  and livesOnCampus
  and pid not in (select pid from worktable);
-- Should be 0.

-- Did we get all of the non-excluded off-campus commuters in the work table?
select count(pid)
from PeopleToBeTested
where isStudent
  and not livesOnCampus
  and pid not in (select pid from worktable);
-- Should be 0.

-- Did we get all of the non-excluded employees in the work table?
select count(pid)
from PeopleToBeTested
where isEmployee
  and not isStudent
  and not livesOnCampus
  and pid not in (select pid from worktable);
-- Should be 0.


-- Look at the work table again, to verify the mixture of employees and commuters and on-campus students.
-- AKA the "Traffic report".
select "inviteDate", "monitorGroup", dormBuilding, count(pid)
from worktable
group by "inviteDate", "monitorGroup", dormBuilding
order by "inviteDate" ASC,
         dormBuilding ASC;


-- If everything looks good, make the files for the IT Team.
copy (select '' as "monitorGroup",
				 dormBuilding,  
				 pid, 
				 coalesce(email1, '') as "email1", 
				 '' as "inviteDateText",
				 '' as "inviteDate", 
				 '' as "unused2",
				 lastName, 
				 firstName, 
				 coalesce(phone1, '') as "phone1", 
				 coalesce(phone2, '') as "phone2",
				 coalesce(email2, '') as "email2",       
				 dormFloor,
				 dormRoom,
				 major
      from workTable
      order by "inviteDate" ASC,
               dormBuilding ASC,		
               lastName ASC)
to '//Users/shared/COVIDscreening/2021weekX.csv' CSV;

-- Make a backup of the most recent invites/worktable
select *
into weekX
from worktable;


-- Finally, append the worktable to our archive of all invites.
-- BUT MAKE SURE this is the FINAL version of this table, the one
-- that's getting e-mailed to our communitry, because this is how
-- we're going to track invite history.

-- Check the count before.
select count(*)
from InviteHistory;

-- Append the data.
insert into InviteHistory
select "monitorGroup",
       dormBuilding,  
       pid, 
       coalesce(email1, '') as "email1", 
       "inviteDateText",
       "inviteDate", 
       '' as "unused2",
       lastName, 
       firstName, 
       coalesce(phone1, '') as "phone1", 
       coalesce(phone2, '') as "phone2",
       coalesce(email2, '') as "email2",       
       dormFloor,
       dormRoom,
       major
from workTable;

-- And check the count after.
select count(*)
from InviteHistory;
