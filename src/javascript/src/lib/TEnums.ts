
enum msgStatus { 
    kBadAddress = 0,
    kProcessed = 1,
    kProcessedNoChange = 2,
    kBadParameters = 4,
    kCreateFailure = 8,
}

enum objState {
    kClean = 0,
    kNewObject = 1,
    kModified = 2,
    kSubModified = 4,
    kMasterModified = 8, 
}

enum penStyle {
    kSolid = 0,
    kDash,
    kDot,
    kDashDot,
    kDashDotDot,
    kLast
}

enum brushStyle{
    kNone,
    kSolid,
}

enum effect {
    kNone,
    kBlur,
    kColorize,
    kShadow,
}