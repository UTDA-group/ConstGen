/*! @file sym_detect/SymDetect.h
    @brief Detect symmetric patterns.
    @author Mingjie Liu
    @date 11/24/2018
*/
#ifndef __SYMDETECT_H__
#define __SYMDETECT_H__

#include "db/Netlist.h"
#include "db/MosPair.h"
#include "sym_detect/Pattern.h"
#include <vector>

PROJECT_NAMESPACE_BEGIN
/*! @class SymDetect
    @brief SymDetect class
*/
class SymDetect
{
public:
/*! @brief Constructor
    Only needs netlist as input. 
    Pattern class inherently constructed.
    @param netlist Netlist class.
 */
    explicit SymDetect(const Netlist & netlist)
        : _netlist(netlist), _pattern(Pattern(netlist))
    {}

/*! @brief Hierarchy symmetry detection. 
    
    Output would contain 2 levels of hierarchy. symGroup
    is a vector of std::vector<MosPair> oneGroup. Where 
    oneGroup is a group of MosPair in the same symmetry 
    group. Each MosPair should follow a MosPattern. 

    @param symGroup Detected symmetry groups of netlist.
    @see MosPattern
    @see MosPair
*/
    void                        hiSymDetect(std::vector<std::vector<MosPair>> & symGroup) const;

private:
    const Netlist & _netlist;
    Pattern   _pattern;

/*! @brief Return pattern of MosPair. */
    MosPattern                  MosPairPtrn(MosPair & obj) const;
///    bool                        endSrch(IndexType mosId, PinType pinType) const;
/*! @brief Check if pair already reached. */
    bool                        existPair(std::vector<MosPair> & library, IndexType instId1, IndexType instId2) const;
/*! @brief Check if pair already reached. */
//    bool                        existPair(std::vector<MosPair> & library, IndexType instId1, IndexType instId2) const;
/*! @brief Return true if end of search path. 

    Current end search terminations:
    (1) DIFF_SOURCE reached through DRAIN
    (2) LOAD, CROSS_LOAD 
    (3) gate connected pairs 
*/
    bool                        endSrch(MosPair & obj) const;
/*! @brief Return true if a valid pair. 

    Valid pairs have following attributes:
    (1) Reached through same PinType
    (2) Not reached through gate
    (3) Valid MosPattern

    @param instId1 Reached pair instId1
    @param instId2 Reached pair instId2
    @param srchPinId1 instId1 reached by srchPinId1.
    @param srchPinId2 instId2 reached by srchPinId2.
*/
    bool                        validSrchObj(IndexType instId1, IndexType instId2, 
                                             IndexType srchPinId1, IndexType srchPinId2) const;
/*! @brief Push next valid MosPair to dfsStack.

    This function push valid pairs that could be reached 
    from currObj to dfsStack. It also removes reached DIFF_SOURCE
    MosPair from diffPairSrc.

    @see inVldDiffPairSrch.
    @param dfsVstPair All current visited MosPair
    @param dfsStack Stack to store to visit MosPair
    @param currObj Current MosPair under visit
    @param diffPairSrc All DFS sources
*/
    void                        pushNextSrchObj(std::vector<MosPair> & dfsVstPair, std::vector<MosPair> & dfsStack, 
                                                MosPair & currObj, std::vector<MosPair> & diffPairSrc) const;
/*! @brief Get srchPatrn MosPair connected to netId.

    Find MosPair that follow srchPatrn. These
    MosPair are appended to diffPair. Used to 
    get valid DFS source. srchPatrn inputs commonly
    are DIFF_SOURCE and CROSS_LOAD.
    Currently pairs should follow:
    (1) Have MosPattern srchPatrn
    (2) source connected to netId
    (3) MosType::DIFF

    @param netId Source should be connected to netId.
    @param diffPair Stored output vector.
*/
    void                        getPatrnNetConn(std::vector<MosPair> & diffPair, IndexType netId,
                                                MosPattern srchPatrn) const;
/*! @brief Get valid DFS source of netlist.

    Iterate all signal nets for getPatrnNetConn.
    Commonly srchPatrn are DIFF_SOURCE and 
    CROSS_LOAD.
    This would return all DFS sources.

    @see getDiffPairNetConn
    @param diffPair Store the output vector
*/
    void                        getDiffPair(std::vector<MosPair> & diffPair) const;
/*! @brief DFS search with given source. Visited MosPair are stored.

    Search for symmetry patterns in DFS manner with search source as diffPair.
    Store visited valid MosPair at dfsVstPair. diffPairSrch are needed as input
    to invalidate reached sources. dfsVstPair would be in the same hierarchy 
    symmetry group.

    @see pushNextSrchObj
    @param[out] dfsVstPair Vector to store all visited MosPair
    @param[in] diffPair DFS search source
    @param[in] diffPairSrch Vector of all stored DFS search source
*/
    void                        dfsDiffPair(std::vector<MosPair> & dfsVstPair, MosPair & diffPair, 
                                             std::vector<MosPair> & diffPairSrch) const;
/*! @brief Invalidate visited pairs from sources.

    If a MosPair have already been visited and is 
    a DFS source, it should be invalidated
    as a DFS search source to avoid revisiting.

    @param diffPairSrch Vector of all DFS sources.
    @param currPair MosPair to invalidate.
*/
    void                        inVldDiffPairSrch(std::vector<MosPair> & diffPairSrch, MosPair & currPair) const;
}; 

PROJECT_NAMESPACE_END

#endif
