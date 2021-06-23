#include <stivale2.h>

static void *GetTag( StivaleEnvironment environment, uint64_t tagIdentifier )
{
    for (struct stivale2_tag* tag = (void *)environment->systemInformation->tags; tag != 0; tag = (void *)tag->next)
        if (tag->identifier == tagIdentifier)
            return tag;

    return 0;
}

void PopulateStivaleEnvironment( StivaleEnvironment environment, Stivale2SystemInformation systemInformation )
{
    environment->systemInformation = systemInformation;

    environment->GetTag = GetTag;
}

