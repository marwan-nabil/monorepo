#pragma once

enum lint_job_type
{
    LJT_FILE_LIST,
    LJT_DIRECTORY
};

struct lint_job
{
    lint_job_type JobType;

    u32 NumberOfFiles;
    char **FilePaths;

    char *DirectoryPath;

    b32 Result;
};

struct lint_job_queue
{
    u32 LintJobCount;
    lint_job *LintJobs;
    volatile i64 NextLintJobIndex;
    volatile i64 TotalJobsDone;
};