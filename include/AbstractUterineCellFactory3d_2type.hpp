#ifndef ABSTRACTUTERINECELLFACTORY3d_2TYPE_HPP
#define ABSTRACTUTERINECELLFACTORY3d_2TYPE_HPP

#include <toml.hpp>
#include "MonodomainProblem.hpp"
#include "ZeroStimulus.hpp"
#include "HodgkinHuxley1952Cvode.hpp"
#include "ChayKeizer1983Cvode.hpp"
#include "Means2022Cvode.hpp"
#include "Tong2014Cvode.hpp"
#include "Roesler2023Cvode.hpp"
#include "iclc_passive_cell_v1Cvode.hpp"

namespace USMC_3D_2TYPE_SYSTEM_CONSTANTS
{
	const std::string CONFIG_DIR = 
		getenv("CHASTE_MODELLING_CONFIG_DIR");
	const std::string GENERAL_PARAM_FILE = 
		"3d_params.toml";
	const std::string GENERAL_2TYPE_PARAM_FILE = 
		"3d_2type_params.toml";		
}

class AbstractUterineCellFactory3d_2type : public AbstractCardiacCellFactory<3>
{
protected:
	std::string mpCell_type;
	std::string mpCell2_type;
	std::unordered_map<std::string, float> mpCell_parameters;
	std::unordered_map<std::string, float> mpCell2_parameters;
	unsigned short int mpCell_id; // 0 = HH, 1 = CK, 2 = Means, 3 = Tong, 4 = Roesler, added 5 = passive
	unsigned short int mpCell2_id;	

public:
	AbstractUterineCellFactory3d_2type();
	AbstractCvodeCell* CreateCardiacCellForTissueNode(Node<3>* pNode);
	std::string GetCellType();
	std::string GetCell2Type();
	virtual void ReadParams(std::string general_param_file);
	//virtual void ReadParams2Type(std::string general_2type_param_file);
	virtual void ReadCellParams(std::string cell_param_file);
	virtual void ReadCell2Params(std::string cell_param_file);
	virtual void PrintParams();
	virtual void WriteLogInfo(std::string log_file);

};

#endif // ABSTRACTUTERINECELLFACTORY3d_HPP

