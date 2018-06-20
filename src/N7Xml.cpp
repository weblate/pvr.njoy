

#include "N7Xml.h"

#include <p8-platform/util/StringUtils.h>
#include "tinyxml.h"
#include "util/XMLUtils.h"

using namespace ADDON;

bool CCurlFile::Get(const std::string &strURL, std::string &strResult)
{
  void* fileHandle = XBMC->OpenFile(strURL.c_str(), 0);
  if (fileHandle)
  {
    char buffer[1024];
    while (XBMC->ReadFileString(fileHandle, buffer, 1024))
      strResult.append(buffer);
    XBMC->CloseFile(fileHandle);
    return true;
  }
  return false;
}

N7Xml::N7Xml(void) :
        m_connected(false)
{
  list_channels();
}

N7Xml::~N7Xml(void)
{
  m_channels.clear();
}

int N7Xml::getChannelsAmount()
{ 
  return m_channels.size();
}

void N7Xml::list_channels()
{
  std::string strUrl;
  strUrl = StringUtils::Format("http://%s:%i/n7channel_nt.xml", g_strHostname.c_str(), g_iPort);
  std::string strXML;

  CCurlFile http;
  if(!http.Get(strUrl, strXML))
  {
    XBMC->Log(LOG_DEBUG, "N7Xml - Could not open connection to N7 backend.");
  }
  else
  {
    TiXmlDocument xml;
    xml.Parse(strXML.c_str());
    TiXmlElement* rootXmlNode = xml.RootElement();
    if (rootXmlNode == NULL)
      return;
    TiXmlElement* channelsNode = rootXmlNode->FirstChildElement("channel");
    if (channelsNode)
    {
      XBMC->Log(LOG_DEBUG, "N7Xml - Connected to N7 backend.");
      m_connected = true;
      int iUniqueChannelId = 0;
      TiXmlNode *pChannelNode = NULL;
      while ((pChannelNode = channelsNode->IterateChildren(pChannelNode)) != NULL)
      {
        std::string strTmp;
        PVRChannel channel;

        /* unique ID */
        channel.iUniqueId = ++iUniqueChannelId;

        /* channel number */
        if (!XMLUtils::GetInt(pChannelNode, "number", channel.iChannelNumber))
          channel.iChannelNumber = channel.iUniqueId;

        /* channel name */
        if (!XMLUtils::GetString(pChannelNode, "title", strTmp))
          continue;
        channel.strChannelName = strTmp;

        /* icon path */
        const TiXmlElement* pElement = pChannelNode->FirstChildElement("media:thumbnail");
        channel.strIconPath = pElement->Attribute("url");

        /* channel url */
        if (!XMLUtils::GetString(pChannelNode, "guid", strTmp))
          channel.strStreamURL = "";
        else
          channel.strStreamURL = strTmp;

        m_channels.push_back(channel);
      }
    }
  }
}


PVR_ERROR N7Xml::requestChannelList(ADDON_HANDLE handle, bool bRadio)
{
  if (m_connected)
  {
    std::vector<PVRChannel>::const_iterator item;
    PVR_CHANNEL tag;
    for( item = m_channels.begin(); item != m_channels.end(); ++item)
    {
      const PVRChannel& channel = *item;
      memset(&tag, 0 , sizeof(tag));

      tag.iUniqueId       = channel.iUniqueId;
      tag.iChannelNumber  = channel.iChannelNumber;
      strncpy(tag.strChannelName, channel.strChannelName.c_str(), sizeof(tag.strChannelName) - 1);
      strncpy(tag.strIconPath, channel.strIconPath.c_str(), sizeof(tag.strIconPath) - 1);

      XBMC->Log(LOG_DEBUG, "N7Xml - Loaded channel - %s.", tag.strChannelName);
      PVR->TransferChannelEntry(handle, &tag);
    }
  }
  else
  {
    XBMC->Log(LOG_DEBUG, "N7Xml - no channels loaded");
  }

  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR N7Xml::GetChannelStreamProperties(const PVR_CHANNEL* channel, PVR_NAMED_VALUE* props, unsigned int* prop_size)
{
  if (*prop_size < 2)
    return PVR_ERROR_INVALID_PARAMETERS;

  if (!m_channels.empty())
  {
    std::vector<PVRChannel>::const_iterator item;
    for (item = m_channels.begin(); item != m_channels.end(); ++item)
    {
       const PVRChannel& chan = *item;
       if (chan.iUniqueId == channel->iUniqueId)
       {
         strncpy(props[0].strName, PVR_STREAM_PROPERTY_STREAMURL, sizeof(props[0].strName) - 1);
         strncpy(props[0].strValue, chan.strStreamURL.c_str(), sizeof(props[0].strValue) - 1);
         strncpy(props[1].strName, PVR_STREAM_PROPERTY_ISREALTIMESTREAM, sizeof(props[1].strName) - 1);
         strncpy(props[1].strValue, "true", sizeof(props[1].strValue) - 1);
         *prop_size = 2;
         return PVR_ERROR_NO_ERROR;
       }
    }
  }
  return PVR_ERROR_UNKNOWN;
}

