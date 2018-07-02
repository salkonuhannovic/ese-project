All commands are to be excuted in *Package Manager Console* inside Visual Studio.

**Add new migration**

    Add-Migration <NewMigrationName> -Project ese.smarthome.core -StartupProject ese.smarthome -Output Db\Migrations

**Remove migration**

    Remove-Migration -Project ese.smarthome.core -StartupProject ese.smarthome

**Drop database**

    Drop-Database -Project ese.smarthome.core -StartupProject ese.smarthome

**Apply pending migrations / Create database if it does not exist**

    Update-Database -Project ese.smarthome.core -StartupProject ese.smarthome