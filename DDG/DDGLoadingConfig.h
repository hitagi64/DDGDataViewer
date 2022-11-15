#ifndef DDGLOADINGCONFIG_H
#define DDGLOADINGCONFIG_H


class DDGLoadingConfig
{
public:
    // For games like Profesional 2 and Shinkansen
    bool useFixedPoint = false;

    // Keep loaded file data in memory so it can be used to save formats
    //  that can't be saved yet.
    bool keepLoadedData = false;
};

#endif // DDGLOADINGCONFIG_H
