/*IlwisObjects is a framework for analysis, processing and visualization of remote sensing and gis data
Copyright (C) 2018  52n North

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.*/

#include "kernel.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "version.h"
#include "ilwisdata.h"
#include "symboltable.h"
#include "commandhandler.h"
#include "operationmetadata.h"
#include "geos/geom/Coordinate.h"
#include "location.h"
#include "ilwiscoordinate.h"
#include "box.h"
#include "workflownode.h"
#include "junctionNode.h"
#include "operationnode.h"
#include "conditionNode.h"
#include "rangenode.h"
#include "rangejunctionnode.h"
#include "ilwisobjectconnector.h"
#include "workflow.h"
#include "ilwis4workflowconnector.h"


using namespace Ilwis;
using namespace Ilwis4C;

ConnectorInterface *Ilwis4WorkflowConnector::create(const Ilwis::Resource &resource, bool load, const IOOptions& options)
{
	return new Ilwis4WorkflowConnector(resource, load, options);
}

Ilwis4WorkflowConnector::Ilwis4WorkflowConnector(const Ilwis::Resource &resource, bool load, const IOOptions& options) : Ilwis4Connector(resource, load, options)
{
	_version = 1;
}

bool Ilwis4WorkflowConnector::store(IlwisObject *obj, const IOOptions &options)
{
	QJsonArray objects;
	QJsonObject jroot, jwf;

	IOOptions newOptions = options;
	newOptions.addOption("version", _version);

	store(obj, newOptions, jwf);
	jroot.insert("ilwisobject", jwf);
	objects.append(jroot);

	flush(obj, objects);

	storeData(obj, options);

	return true;
}

bool Ilwis4WorkflowConnector::loadNode(QJsonObject jnode, Workflow *wf, SPWorkFlowNode &node) {

	QString nname = jnode["name"].toString();
	QString label = jnode["label"].toString();
	QString desc = jnode["description"].toString();
	qint32 nodeid = jnode["nodeid"].toInt();
	bool isWorkflow = jnode["isworkflow"].toBool();
	QString syntax = jnode["syntax"].toString();
	QString provider = jnode["provider"].toString();
	qint32 type = jnode["type"].toInt();
	bool isCollapsed = jnode["collapsed"].toBool();
	QString sbox = jnode["box"].toString();

	bool valid = true;
	if (type == (qint32)WorkFlowNode::ntRANGEJUNCTION) {
		auto opNode = new RangeJunctionNode(nodeid);
		opNode->name(nname);
		opNode->setDescription(desc);
		node.reset(opNode);
	}
	else  if (type == (qint32)WorkFlowNode::ntOPERATION) {
		if (syntax.size() <= 1)     // invalid operation
			valid = false;      // mark as such; a bit of a hack!
		auto opNode = new OperationNode(nname, desc, nodeid);
		opNode->operation(provider, syntax, isWorkflow);
		node.reset(opNode);
	}
	else if (type == (qint32)WorkFlowNode::ntCONDITION) {
		auto cnode = new WorkFlowCondition();
		cnode->nodeId(nodeid);
		node.reset(cnode);
		qint32 ocount;
		QJsonObject jcondition = jnode["condition"].toObject();
		QJsonArray jtests = jcondition["tests"].toArray();
		for (auto jtestr : jtests) {
			QJsonObject jtest = jtestr.toObject();

			SPWorkFlowNode operationNode;
			loadNode(jtest, wf, operationNode);
			cnode->addTest(operationNode);
			operationNode->conditionIdOfTest(cnode->id());
		}
		QJsonArray joperations = jcondition["operations"].toArray();
		for (auto joperr : joperations) {
			QJsonObject joperation = joperr.toObject();
			SPWorkFlowNode operationNode;
			loadNode(joperation, wf, operationNode);
			cnode->addSubNode(operationNode, "operations");
			operationNode->owner(node);
		}
	}
	else if (type == (qint32)WorkFlowNode::ntRANGE) {
		auto rnode = new RangeNode();
		rnode->nodeId(nodeid);
		node.reset(rnode);
		QJsonObject jrangeNode = jnode["range"].toObject();
		QString rangedef = jrangeNode["rangedef"].toString();
		rnode->setRangeDefinition(rangedef, wf);
		QJsonArray joperations = jrangeNode["operations"].toArray();
		for (auto jor : joperations) {
			QJsonObject joperation = jor.toObject();
			SPWorkFlowNode operationNode;
			loadNode(joperation, wf, operationNode);
			rnode->addSubNode(operationNode, "operations");
			operationNode->owner(node);
		}
		QJsonArray jjunctions = jrangeNode["junctions"].toArray();;
		for (auto jor : jjunctions) {
			QJsonObject jjunction = jor.toObject();
			SPWorkFlowNode junctionNode;
			loadNode(jjunction, wf, junctionNode);
			rnode->addSubNode(junctionNode, "junctions");
			junctionNode->owner(node);
		}
	}
	else if (type == (qint32)WorkFlowNode::ntJUNCTION) {
		auto cnode = new JunctionNode();
		cnode->nodeId(nodeid);
		node.reset(cnode);

	}
	node->collapsed(isCollapsed);
	node->label(label);
	node->box(sbox);

	QJsonArray jparms = jnode["parameters"].toArray();
	int idx = 0;
	for (auto jpor : jparms) {
		QJsonObject jparm = jpor.toObject();
		//int order = jparm["order"].toInt();
		QString pname = jparm["name"].toString();
		QString pdesc = jparm["description"].toString();
		QString plabel = jparm["label"].toString();
		QString flowlabel = jparm["flowlabel"].toString();
		QString vt = jparm["valuetype"].toString();
		IlwisTypes valueType = vt.toULongLong();
		qint32 state = jparm["state"].toInt();

		WorkFlowParameter wp(idx, node->id(), pname, pdesc);
		wp.label(plabel);
		wp.flowLabel(flowlabel);
		wp.addSyntax(jparm["syntax"].toString());
		wp.needsQuotes(jparm["needsquotes"].toBool());
		QString value;
		Ilwis4Connector::loadMetaData(jparm, valueType, value);
		wp.value(value, valueType, (WorkFlowParameter::ParameterKind)state);
		node->addInput(wp, idx++);

	}
	return true;
}
bool Ilwis4WorkflowConnector::loadMetaData(IlwisObject* obj, const IOOptions& options, const QJsonValue& jvalue) {


	Ilwis4Connector::loadMetaData(obj, options, jvalue);

	Workflow *workflow = static_cast<Workflow *>(obj);

	workflow->scale(jvalue["scale"].toDouble());
	workflow->translation(jvalue["translationx"].toInt(), jvalue["translationt"].toInt(), false);

	std::vector<SPWorkFlowNode> independenNodes;
	std::vector<quint64> invalids;

	QJsonObject jgraph = jvalue["graph"].toObject();
	QJsonArray jnodes = jgraph["nodes"].toArray();
	for (auto jnoder : jnodes) {
		QJsonObject jnode = jnoder.toObject();
		SPWorkFlowNode node;
		bool valid = Ilwis4WorkflowConnector::loadNode(jnode, workflow, node);

		if (!valid)
			invalids.push_back(node->id());
		workflow->addNode(node);
		independenNodes.push_back(node);
	}

	QJsonArray jlinks = jgraph["links"].toArray();
	for (int i = 0; i < jlinks.size(); ++i) {
		QJsonObject jlink = jlinks[i].toObject();
		SPWorkFlowNode node = independenNodes[i];
		loadNodeLinks(jlink, node, workflow);
	}

	for (qint32 i = 0; i < invalids.size(); i++)
		workflow->removeNode(invalids[i]);

	workflow->updateIdCounter();

	return true;


}
bool Ilwis4WorkflowConnector::store(IlwisObject *obj, const IOOptions& options, QJsonObject& jwf) {
	
	Ilwis4Connector::store(obj, options, jwf);
	Workflow *workflow = static_cast<Workflow *>(obj);
	jwf.insert("scale", workflow->scale());
	jwf.insert("translationx", workflow->translation().first);
	jwf.insert("translationy", workflow->translation().second);

	const std::vector<SPWorkFlowNode>& graph = workflow->graph();

	QJsonObject jgraph;
	QJsonArray jnodes, jnodeLinks;

	for (const SPWorkFlowNode node : graph) {
		QJsonObject jnode;
		storeNode(node, jnode, options);
		jnodes.append(jnode);
	}

	for (const SPWorkFlowNode& node : graph) {
		QJsonObject jnodeLink;
		storeNodeLinks(node, jnodeLink, options);
		jnodeLinks.append(jnodeLink);
	}
	jgraph.insert("links", jnodeLinks);
	jgraph.insert("nodes", jnodes);
	jwf.insert("graph", jgraph);


	return true;
}

void Ilwis4WorkflowConnector::loadNodeLinks(const  QJsonObject& jnode, SPWorkFlowNode& node, Workflow *workflow) {

	if (node->type() == WorkFlowNode::ntCONDITION) {
		std::shared_ptr<WorkFlowCondition> condition = std::static_pointer_cast<WorkFlowCondition>(node);
		QJsonArray jclinks = jnode["testlinks"].toArray();
		QJsonArray jolinks = jnode["operationlinks"].toArray();
		for (int i = 0; i < jclinks.size(); ++i) {
			QJsonObject jclink = jclinks[i].toObject();
			WorkFlowCondition::Test test = condition->test(i);
			loadNodeLinks(jclink, test._operation, workflow);
		}
		auto subnodes = condition->subnodes("operations");
		for (int j = 0; j < subnodes.size(); ++j) {
			SPWorkFlowNode operationNode = subnodes[j];
			QJsonObject jop = jolinks[j].toObject();
			loadNodeLinks(jop, operationNode, workflow);
		}
	}
	if (node->type() == WorkFlowNode::ntRANGE) {
		// loadNodeLinks(node, workflow);

		QJsonArray jclinks = jnode["conditionlinks"].toArray();
		QJsonArray jolinks = jnode["operationlinks"].toArray();
		auto subnodes = node->subnodes("operations");
		for (int i = 0; i < jolinks.size(); ++i) {
			QJsonObject jolink = jolinks[i].toObject();
			SPWorkFlowNode operationNode = subnodes[i];
			loadNodeLinks(jolink, operationNode, workflow);
		}

		auto junctions = node->subnodes("junctions");
		for (int i = 0; i < jclinks.size(); ++i) {
			QJsonObject jclink = jclinks[i].toObject();
			SPWorkFlowNode junctionNode = junctions[i];
			loadNodeLinks(jclink, junctionNode, workflow);
		}
	}
	if (jnode.contains("parmlinks")) {
		QJsonArray jparmlinks = jnode["parmlinks"].toArray();

		for (qint32 j = 0; j < jparmlinks.size(); ++j) {
			QJsonObject jparmLink = jparmlinks[j].toObject();
			qint32 outParmindex = jparmLink.contains("outputindex") ? jparmLink["outputindex"].toInt() : iUNDEF;
			QJsonValue jnodeid = jparmLink["sourceid"];
			if (jnodeid != QJsonValue::Undefined) {
				SPWorkFlowNode prevNode = workflow->nodeById(jnodeid.toInt());
				if (prevNode)
					node->inputRef(j).inputLink(prevNode, outParmindex);
			}
		}
	}
}
void Ilwis4WorkflowConnector::storeNodeLinks(const SPWorkFlowNode& node, QJsonObject& jnode, const IOOptions &options) {
	if (node->type() == WorkFlowNode::ntCONDITION) {
		std::shared_ptr<WorkFlowCondition> condition = std::static_pointer_cast<WorkFlowCondition>(node);
		qint32 sz = (qint32)condition->subnodes("tests").size();
		QJsonArray jclinks;
		// these two members are not needed but are written to make the jspn more readable.
		for (qint32 t = 0; t < sz; ++t) {
			QJsonObject jlink;
			WorkFlowCondition::Test test = condition->test(t);
			storeNodeLinks(test._operation, jlink, options);
			jclinks.append(jlink);
		}
		auto operations = condition->subnodes("operations");
		QJsonArray jolinks;
		for (qint32 o = 0; o < operations.size(); ++o) {
			QJsonObject jolink;
			storeNodeLinks(operations[o], jolink, options);
			jolinks.append(jolink);
		}
		if ( jclinks.size() > 0)
			jnode.insert("testlinks", jclinks);
		if ( jolinks.size() > 0)
			jnode.insert("operationlinks", jolinks);
	}
	if (node->type() == WorkFlowNode::ntRANGE) {
		auto operations = node->subnodes("operations");
		QJsonArray jolinks;
		for (qint32 o = 0; o < operations.size(); ++o) {
			QJsonObject jolink;
			storeNodeLinks(operations[o], jolink, options);
			jolinks.append(jolink);
		}
		auto junctions = node->subnodes("junctions");
		QJsonArray jclinks;
		for (qint32 o = 0; o < junctions.size(); ++o) {
			QJsonObject jlink;
			storeNodeLinks(junctions[o], jlink, options);
			jclinks.append(jlink);
		}
		if ( jclinks.size() > 0)
			jnode.insert("conditionlinks", jclinks);
		if ( jolinks.size() > 0)
			jnode.insert("operationlinks", jolinks);
	}
	int count = node->inputCount();
	// junctions have 3 or 4 parameters; but inputcount returns only 1 as the link to the condition is the only explicit parameter;
	//links to operations are implicit. Needed for saving though so we overrule here
	if (node->type() == WorkFlowNode::ntJUNCTION)
		count = 3;
	if (node->type() == WorkFlowNode::ntRANGEJUNCTION)
		count = 4;

	QJsonArray jparmLinks;

	for (qint32 i = 0; i < count; ++i) {
		QJsonObject jparmlink;
		WorkFlowParameter& wp = node->inputRef(i);
		int idx = wp.outputParameterIndex();
		if ( idx != iUNDEF)
			jparmlink.insert("outputindex", idx);
		if (wp.isValid()) {
			if (wp.inputLink())
				jparmlink.insert("sourceid", (qint32)wp.inputLink()->id());
		}
		jparmLinks.append(jparmlink); 
	}
	if (jparmLinks.size() > 0) {
		jnode.insert("targetid", (int)node->id());
		jnode.insert("name", node->name());
		jnode.insert("parmlinks", jparmLinks);
	}
}

void Ilwis4WorkflowConnector::storeNode(const SPWorkFlowNode& node, QJsonObject& jnode, const IOOptions &options)  {
	jnode.insert("name", node->name());
	jnode.insert("label", node->label());
	jnode.insert("description", node->description());
	jnode.insert("nodeid", (qint64)node->id());
	jnode.insert("box", node->box().toString());

	IOperationMetaData op = node->operation();
	if (op.isValid()) {
		QString syntax = op->resource()["syntax"].toString();
		QString provider = op->resource()["namespace"].toString();
		jnode.insert("syntax", syntax);
		jnode.insert("provider", provider);
		jnode.insert("isworklfow", op->ilwisType() == itWORKFLOW);
	}
	jnode.insert("type", node->type());
	jnode.insert("collapsed", node->collapsed());
	if (node->type() == WorkFlowNode::ntCONDITION) {
		QJsonObject jcondition;
		std::shared_ptr<WorkFlowCondition> condition = std::static_pointer_cast<WorkFlowCondition>(node);
		QJsonArray jtests;
		for (qint32 t = 0; t < condition->subnodes("tests").size(); ++t) {
			QJsonObject jtest;
			WorkFlowCondition::Test test = condition->test(t);
			storeNode(test._operation, jtest, options);
			jtests.append(jtest);
		}
		auto operations = condition->subnodes("operations");
		QJsonArray joperations;
		for (int o = 0; o < operations.size(); ++o) {
			QJsonObject joperation;
			storeNode(operations[o], joperation, options);
			joperations.append(joperation);
		}
		jcondition.insert("tests", jtests);
		jcondition.insert("operations", joperations);
		jnode.insert("condition", jcondition);
	}
	if (node->type() == WorkFlowNode::ntRANGE) {
		std::shared_ptr<RangeNode> range = std::static_pointer_cast<RangeNode>(node);
		QString rangedef = range->rangeDefinition();
		QJsonObject jrange;
		jrange.insert("rangedef", rangedef);

		auto operations = node->subnodes("operations");
		QJsonArray joperations;
		for (qint32 o = 0; o < operations.size(); ++o) {
			QJsonObject joperation;
			storeNode(operations[o], joperation, options);
			joperations.append(joperation);
		}
		auto junctions = node->subnodes("junctions");
		QJsonArray jjunctions;
		for (qint32 o = 0; o < junctions.size(); ++o) {
			QJsonObject jjunction;
			storeNode(junctions[o], jjunction, options);
			jjunctions.append(jjunction);
		}
		jrange.insert("junctions", jjunctions);
		jrange.insert("operations", joperations);
		jnode.insert("range", jrange);
	}

	qint32 count = node->inputCount();
	// junctions have 3 or 4 parameters; but inputcount returns only 1 as the link to the condition is the only explicit parameter;
	//links to operations are implicit. Needed for saving though so we overrule here
	if (node->type() == WorkFlowNode::ntJUNCTION)
		count = 3;
	if (node->type() == WorkFlowNode::ntRANGEJUNCTION)
		count = 4;


	QJsonArray jparameters;
	for (qint32 i = 0; i < count; ++i) {
		WorkFlowParameter& wp = node->inputRef(i);

		QJsonObject jparm;

		jparm.insert("order", wp.order());
		jparm.insert("name", wp.name());
		jparm.insert("description", wp.description());
		jparm.insert("label", wp.label());
		jparm.insert("flowlabel", wp.flowLabel());
		jparm.insert("valuetype", QString::number(wp.valueType()));
		jparm.insert("state", wp.state());
		jparm.insert("syntax", wp.syntax());
		jparm.insert("needsquotes", wp.needsQuotes());

		Ilwis4Connector::store(wp.value(), wp.valueType(), jparm, options);
		jparameters.append(jparm);

	}
	jnode.insert("parameters", jparameters);
}

bool Ilwis4WorkflowConnector::storeData(IlwisObject *obj, const IOOptions &options) {


	return true;
}

bool Ilwis4WorkflowConnector::loadMetaData(IlwisObject *obj, const IOOptions &options)
{
	QFileInfo inf = _resource.url(true).toLocalFile();
	QFile file;
	file.setFileName(inf.absoluteFilePath());
	if (file.open(QIODevice::ReadOnly)) {
		QString meta = file.readAll();
		QJsonDocument doc = QJsonDocument::fromJson(meta.toUtf8());
		if (!doc.isNull()) {
			QJsonArray jobjects = doc.array();
			QJsonValue jvalue = jobjects.at(0);
			return loadMetaData(obj, options, jvalue["ilwisobject"]);
		}
	}
	return true;
}

bool Ilwis4WorkflowConnector::loadData(IlwisObject* obj, const IOOptions& options) {

	return true;
}
